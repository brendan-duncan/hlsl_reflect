#include "parser.h"

#include "../../util/string_util.h"
#include "../../ast/type_flags.h"
#include "parser/effect_state.h"
#include "parse_exception.h"

namespace reader {
namespace hlsl {

Parser::Parser(const std::string_view& source)
  : _scanner(source) {
}

ast::Ast* Parser::parse() {
  _ast = new ast::Ast();

  ast::Root* root = _ast->root();

  ast::Statement* lastStatement = nullptr;
  while (!isAtEnd()) {
    ast::Statement* statement = nullptr;
    try {
      statement = parseTopLevelStatement();
    } catch (ParseException e) {
      std::cerr << "Error: "  << e.message << std::endl;
      std::cerr << "  Token: " << e.token.lexeme() << std::endl;
      std::cerr << "  Line: " << _scanner.absoluteLine() << std::endl;
      delete _ast;
      _ast = nullptr;
      return nullptr;
    }
    if (statement == nullptr) {
      if (isAtEnd()) {
        break;
      }
      std::cerr << "Error Line: " << _scanner.absoluteLine() << std::endl;
      delete _ast;
      _ast = nullptr;
      return nullptr;
    }
    if (root->statements == nullptr) {
      root->statements = statement;
    }
    if (lastStatement != nullptr) {
      lastStatement->next = statement;
    }
    lastStatement = statement;
    // If it's a compound statement, we need to find the last statement in the list
    while (lastStatement->next != nullptr) {
      lastStatement = lastStatement->next;
    }
  }

  return _ast;
}

bool Parser::isAtEnd() {
  return (_pending.empty() && _scanner.isAtEnd()) || peekNext().type() == TokenType::EndOfFile;
}

Token Parser::consume(TokenType type, const char* message) {
  if (check(type)) {
    return advance();
  }
  throw ParseException(peekNext(), message);
}

Token Parser::advance() {
  if (!_pending.empty()) {
    Token t = _pending.front();
    _pending.pop_front();
    if (!_restore.empty()) {
      _restore.back().push_back(t);
    }
    return t;
  }

  if (!_restore.empty()) {
    Token t = _scanner.scanNext();
    _restore.back().push_back(t);
    return t;
  }
  
  return _scanner.scanNext();
}

const Token& Parser::peekNext() {
  if (!_pending.empty()) {
    return _pending.front();
  }
  _pending.push_back(_scanner.scanNext());
  return _pending.front();
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(TokenType type) {
  if (isAtEnd()) {
    return false;
  }
  const Token& tk = peekNext();
  return tk.type() == type;
}

ast::Statement* Parser::parseTopLevelStatement() {
  // Discard any solo semicolons
  while (match(TokenType::Semicolon) && !isAtEnd()) {}

  ast::Attribute* attributes = parseAttributes();

  if (match(TokenType::Struct)) {
    ast::StructStmt* structNode = parseStruct();
    if (structNode != nullptr) {
      structNode->attributes = attributes;
    }
    
    ast::Statement* node = structNode;
    if (peekNext().type() == TokenType::Identifier) {
      Token name = advance();
      ast::VariableStmt* var = _ast->createNode<ast::VariableStmt>();
      var->type = _ast->createNode<ast::Type>();
      var->type->baseType = ast::BaseType::Struct;
      var->type->name = structNode->name;
      var->name = name.lexeme();
      node = var;
      while (match(TokenType::Comma)) {
        Token name = consume(TokenType::Identifier, "identifier expected");
        ast::VariableStmt* next = _ast->createNode<ast::VariableStmt>();
        next->type = _ast->createNode<ast::Type>();
        next->type->baseType = ast::BaseType::Struct;
        next->type->name = structNode->name;
        next->name = name.lexeme();
        var->next = next;
        var = next;
      }
    }

    consume(TokenType::Semicolon, "';' expected for struct");
    return node;
  }

  if (check(TokenType::Cbuffer) || check(TokenType::Tbuffer)) {
    Token bufferType = advance();
    ast::BufferStmt* node = parseBuffer();
    node->bufferType = bufferType.type() == TokenType::Cbuffer
        ? ast::BufferType::Cbuffer
        : ast::BufferType::Tbuffer;
    if (node != nullptr) {
      node->attributes = attributes;
    }
    return node;
  }

  if (match(TokenType::Typedef)) {
    ast::TypedefStmt* node = parseTypedef();
    if (node != nullptr) {
      node->attributes = attributes;
    }
    consume(TokenType::Semicolon, "Expected ';' after typedef");
    return node;
  }

  // Global Declaration
  ast::Type* type = parseType(true);
  if (type != nullptr) {
    // Either a variable or function declaration.
    Token identifier = consume(TokenType::Identifier, "identifier expected");

    // function declaration
    if (check(TokenType::LeftParen)) {
      ast::FunctionStmt* func = parseFunctionStmt(type, identifier.lexeme());
      _ast->addFunction(func);
      if (func != nullptr) {
        func->attributes = attributes;
      }
      return func;
    }

    // variable declaration
    ast::Statement* var = parseVariableStmt(type, identifier.lexeme(), attributes);
    consume(TokenType::Semicolon, "Expected ';' after variable declaration");
    // variable declarations can be chained together with ','.
    ast::Statement* firstStmt = var;
    while (var != nullptr) {
      if (var->nodeType == ast::NodeType::VariableStmt) {
        _ast->addGlobalVariable(static_cast<ast::VariableStmt*>(var));
      }
      if (var->next == nullptr) {
        break;
      }
      var = var->next;
    }

    return firstStmt;
  }

  return nullptr;
}

ast::TypedefStmt* Parser::parseTypedef() {
  ast::TypedefStmt* node = _ast->createNode<ast::TypedefStmt>();
  node->type = parseType(true, "typedef type expected");
  node->name = advance().lexeme();
  _typedefs[node->name] = node;
  return node;
}

ast::StructStmt* Parser::parseStruct() {
  ast::StructStmt* s = _ast->createNode<ast::StructStmt>();

  if (peekNext().type() == TokenType::LeftBrace) {
    // anonymous struct
    static int anonStructCount = 0;
    _anonymousStructNames.push_back("__anon_struct_" + std::to_string(anonStructCount++));
    s->name = std::string_view(_anonymousStructNames.back());
  } else {
    Token name = consume(TokenType::Identifier, "struct name expected.");
    consume(TokenType::LeftBrace, "'{' expected for struct");  
    s->name = name.lexeme();
  }

  _structs[s->name] = s;

  ast::Field* lastField = nullptr;
  ast::FunctionStmt* lastMethod = nullptr;

  while (!check(TokenType::RightBrace) && !isAtEnd()) {
    ast::Field* field = parseStructField();

    if (check(TokenType::LeftParen)) {
      // we have a function, so the field parsed above should contain
      //   the return type and function name.  We just need to parse the
      //   body of the function.

      ast::FunctionStmt *method = parseFunctionStmt(field->type, field->name);

      if (s->methods == nullptr) {
        s->methods = method;
      }
      if (lastMethod != nullptr) {
        lastMethod->next = method;
      }
      lastMethod = method;
    
      continue;
    }

    if (s->fields == nullptr) {
      s->fields = field;
    }
    if (lastField != nullptr) {
      lastField->next = field;
    }
    lastField = field;

    // We hit a semicolon, so that's the end of this field declaration
    if (check(TokenType::Semicolon)) {
        consume(TokenType::Semicolon, "';' expected for struct field");
        continue;
    }

    // If we have a comma, then there are more variables for the same
    //   type coming, so consume them.
    if (check(TokenType::Comma)) {
      while (check(TokenType::Comma)) {
        advance(); // consume comma
        ast::Field* nextField = _ast->createNode<ast::Field>();
        nextField->type = field->type;
        nextField->name = advance().lexeme();
        if (match(TokenType::LeftBracket)) {
          // Array field
          nextField->isArray = true;
          nextField->arraySize = parseArraySize();
        }
        lastField->next = nextField;
        lastField = nextField;
      }

      consume(TokenType::Semicolon, "';' expected for struct field");
      continue;
    }
  }

  consume(TokenType::RightBrace, "'}' expected for struct");

  return s;
}

ast::Field* Parser::parseStructField() {
  ast::Field* field = _ast->createNode<ast::Field>();

  Token tk = peekNext();
  if (isInterpolationModifier(tk.type())) {
    field->interpolation = tokenTypeToInterpolationModifier(tk.type());
    advance();
  }

  field->type = parseType(false /* , "struct field type expected" */);
  field->name = advance().lexeme();

  if (match(TokenType::LeftBracket)) {
    // Array field
    field->isArray = true;
    field->arraySize = parseArraySize();
  }

  if (match(TokenType::Colon)) {
    // Struct field has a semantic
    field->semantic = advance().lexeme();
  }

  return field;
}

ast::Attribute* Parser::parseAttributes() {
  // Parse attributes in the forms:
  // [A] statement
  // [A, B] statement
  // [A][B] statement
  // Where: A, B are attributes in the form of:
  // A
  // A(a[,b]*) where a and b are expressions
  ast::Attribute* firstAttribute = nullptr;
  ast::Attribute* lastAttribute = nullptr;;

  if (!match(TokenType::LeftBracket)) {
    return firstAttribute;
  }

  while (!isAtEnd()) {
    ast::Attribute* attribute = _ast->createNode<ast::Attribute>();
    if (firstAttribute == nullptr) {
      firstAttribute = attribute;
    }
    if (lastAttribute != nullptr) {
      lastAttribute->next = attribute;
    }
    lastAttribute = attribute;

    Token name = consume(TokenType::Identifier, "attribute name expected");
    lastAttribute->name = name.lexeme();

    if (match(TokenType::LeftParen)) {
      ast::Expression* firstArg = nullptr;
      ast::Expression* lastArg = nullptr;
      while (!isAtEnd() && !match(TokenType::RightParen)) {
        ast::Expression* arg = parseExpression();
        if (firstArg == nullptr) {
          firstArg = arg;
        }
        if (lastArg != nullptr) {
          lastArg->next = arg;
        }
        lastArg = arg;

        if (!match(TokenType::Comma)) {
          if (!match(TokenType::RightParen)) {
            throw ParseException(peekNext(), "')' or ',' expected for attribute");
          }
          break;
        }
      }
    }

    if (match(TokenType::Comma)) {
      continue;
    }

    if (!match(TokenType::RightBracket)) {
      throw ParseException(peekNext(), "']' or ',' expected for attribute");
      break;
    }

    if (!match(TokenType::LeftBracket)) {
      break;
    }
  }

  return firstAttribute;
}

ast::Expression* Parser::parseAssignmentExpression(ast::Type* type) {
  if (check(TokenType::LeftBrace)) {
    // Array or struct initialization expression (e.g. a = {1, 2, 3})
    if (type->baseType == ast::BaseType::Struct) {
      // Struct initialization
      return parseStructInitialization(type);
    }

    // Array initialization
    return parseArrayInitialization(type);
  }

  ast::Expression* expression = parseLogicalOrExpression();
  if (expression == nullptr) {
    return nullptr;
  }

  // Check for cascading assignment operators (a = b = c)
  if (!isAssignmentOperator(peekNext().type())) {
    return expression;
  }

  ast::Operator op = tokenTypeToAssignmentOperatator(advance().type());

  ast::Expression* value = parseAssignmentExpression(type);
  if (value == nullptr) {
    throw ParseException(peekNext(), "expression expected for assignment");
  }
  ast::AssignmentExpr* assignment = _ast->createNode<ast::AssignmentExpr>();
  assignment->op = op;
  assignment->variable = expression;
  assignment->value = value;
  return assignment;
}

ast::Expression* Parser::parseStructInitialization(ast::Type* type) {
  consume(TokenType::LeftBrace, "'{' expected for struct initialization");

  ast::StructStmt* structType = _structs[type->name];
  if (structType == nullptr) {
    throw ParseException(peekNext(), "unknown struct type");
  }
  ast::Field* field = structType->fields;
  ast::Expression* firstExpression = nullptr;
  ast::Expression* lastExpression = nullptr;
  while (field != nullptr) {
    ast::Expression* expression = parseAssignmentExpression(field->type);
    if (firstExpression == nullptr) {
      firstExpression = expression;
    }
    if (lastExpression != nullptr) {
      lastExpression->next = expression;
    }
    lastExpression = expression;
    field = field->next;
    if (field != nullptr) {
      consume(TokenType::Comma, "',' expected for struct initialization");
    }
  }

  if (check(TokenType::Comma)) {
    advance(); // skip trailing comma
  }

  consume(TokenType::RightBrace, "'}' expected for array initialization");

  ast::StructInitializerExpr* init = _ast->createNode<ast::StructInitializerExpr>();
  init->structType = structType;
  init->fields = firstExpression;
  return init;
}

ast::Expression* Parser::parseArrayInitialization(ast::Type* type) {
  if (match(TokenType::LeftBrace)) {
      // Nested array initialization (e.g. a = {{1, 2}, {3, 4}}
      ast::Expression* firstExpression = parseArrayInitialization(type);
      ast::Expression* lastExpression = firstExpression;

      while (!isAtEnd() && match(TokenType::Comma)) {
        if (check(TokenType::RightBrace)) {
          // Allow trailing comma (e.g. a = {1, 2, 3,})
          break;
        }
        
        ast::Expression* expression = parseArrayInitialization(type);
        if (expression == nullptr) {
          throw ParseException(peekNext(), "expression expected for assignment");
        }
        lastExpression->next = expression;
        lastExpression = expression;
      }

      consume(TokenType::RightBrace, "'}' expected for array initialization");

      ast::ArrayInitializerExpr* init = _ast->createNode<ast::ArrayInitializerExpr>();
      init->elements = firstExpression;

      return init;
  }
  
  return parseExpression();
}

ast::Expression* Parser::parseExpression() {
  return parseLogicalOrExpression();
}

ast::Expression* Parser::parseExpressionList() {
  ast::Expression* firstExpression = parseExpression();
  if (firstExpression == nullptr) {
    return nullptr;
  }
  ast::Expression* lastExpression = firstExpression;

  while (!isAtEnd() && match(TokenType::Comma)) {
    if (check(TokenType::RightBrace) || check(TokenType::RightParen)) {
      break;
    }
    ast::Expression* expression = parseExpression();
    if (expression == nullptr) {
      throw ParseException(peekNext(), "expression expected for expression list");
      break;
    }
    lastExpression->next = expression;
    lastExpression = expression;
  }

  return firstExpression;
}

ast::Field* Parser::parseBufferField() {
  ast::Type* type = parseType(false, "buffer field type expected");

  ast::Field* lastDecl = nullptr;
  ast::Field* firstDecl = nullptr;

  do {
    ast::Field* decl = _ast->createNode<ast::Field>();
    if (firstDecl == nullptr) {
      firstDecl = decl;
    }
    if (lastDecl != nullptr) {
      lastDecl->next = decl;
    }
    lastDecl = decl;

    decl->name = consume(TokenType::Identifier, "identifier expected for declaration").lexeme();
    decl->type = type;

    if (match(TokenType::LeftBracket)) {
      decl->type->array = true;
      decl->type->arraySize = parseExpression();
      consume(TokenType::RightBracket, "']' expected for array declaration");
    }

    if (match(TokenType::Equal)) {
      if (decl->type->array) {
        if (!match(TokenType::LeftBrace)) {
          throw ParseException(peekNext(), "'{' expected for array initializer");
        }
        decl->assignment = parseExpressionList();
        consume(TokenType::RightBrace, "'}' expected for array initializer");
      } else if (isSamplerBaseType(decl->type->baseType)) {
        decl->assignment = parseSamplerState();
      } else {
        decl->assignment = parseExpression();
      }
    }
  } while (match(TokenType::Comma) && !isAtEnd());

  return firstDecl;
}

ast::SamplerState* Parser::parseSamplerState() {
  consume(TokenType::SamplerState, "sampler state expected");
  consume(TokenType::LeftBrace, "'{' expected for sampler state");

  ast::SamplerState* samplerState = _ast->createNode<ast::SamplerState>();

  ast::StateAssignment* lastStateAssignment = nullptr;
  while (!match(TokenType::RightBrace) && !isAtEnd()) {
    ast::StateAssignment* stateAssignment = parseStateAssignment(true, true);
    if (samplerState->stateAssignments == nullptr) {
      samplerState->stateAssignments = stateAssignment;
    }
    if (lastStateAssignment != nullptr) {
      lastStateAssignment->next = stateAssignment;
    }
    lastStateAssignment = stateAssignment;
  }
  
  return samplerState;
}

ast::StateAssignment* Parser::parseStateAssignment(bool isSamplerState, bool isPipelineState) {
  ast::StateAssignment* stateAssignment = _ast->createNode<ast::StateAssignment>();

  stateAssignment->stateName = consume(TokenType::Identifier,
      "identifier expected for state assignment").lexeme();

  const EffectState* state =
      getEffectState(stateAssignment->stateName, isSamplerState, isPipelineState);

  if (state == nullptr) {
    throw ParseException(peekNext(),
        "unknown state assignment " + std::string(stateAssignment->stateName));
  }

  stateAssignment->d3dRenderState = state->d3dRenderState;

  consume(TokenType::Equal, "'=' expected for state assignment");

  const bool expectsExpression = state->values == colorMaskValues;
  const bool expectsInteger = state->values == integerValues;
  const bool expectsFloat = state->values == floatValues;
  const bool expectsBoolean = state->values == booleanValues;

  if (!expectsExpression && !expectsInteger && !expectsFloat && !expectsBoolean) {
    consume(TokenType::Identifier, "identifier expected for state assignment");
  }

  if (state->values == nullptr) {
    Token tk = advance();
    if (tk.type() != TokenType::Identifier || tk.lexeme() != "compile") {
      throw ParseException(tk, "invalid state assignment");
    }

    while (!match(TokenType::Semicolon) && !isAtEnd()) {
      advance();
    }
  } else {
    if (expectsExpression) {
      int mask = 0;
      while (!isAtEnd()) {
        Token tk = advance();
        if (tk.type() == TokenType::IntLiteral) {
          mask |= util::toInt(tk.lexeme());
        } else if (tk.type() == TokenType::Identifier) {
          // color mask value
          const EffectStateValue* stateValue = colorMaskValues;
          while (stateValue->name != nullptr) {
            if (util::stringEqualCaseInsensitive(tk.lexeme(), stateValue->name)) {
              mask |= stateValue->value;
              break;
            }
            ++stateValue;
          }
        } else {
          throw ParseException(tk, "invalid state assignment");
        }
        if (!match(TokenType::Pipe)) {
          break;
        }
      }
    } else if (expectsInteger) {
      Token tk = consume(TokenType::IntLiteral, "integer expected for state assignment");
      stateAssignment->intValue = util::toInt(tk.lexeme());
    } else if (expectsFloat) {
      Token tk = consume(TokenType::FloatLiteral, "float expected for state assignment");
      stateAssignment->floatValue = util::toFloat(tk.lexeme());
    } else if (expectsBoolean) {
      const Token& tk = advance();
      if (tk.type() == TokenType::Identifier) {
        const EffectStateValue* value = getEffectStateValue(tk.lexeme(), state);
        if (value != nullptr) {
          stateAssignment->intValue = value->value;
        }
      } else if (tk.type() == TokenType::IntLiteral) {
        stateAssignment->intValue = util::toInt(tk.lexeme());
      } else {
        throw ParseException(peekNext(), "invalid state assignment");
      }
    } else {
      throw ParseException(peekNext(), "invalid state assignment");
    }
  }

  consume(TokenType::Semicolon, "';' expected for state assignment");

  return stateAssignment;
}

ast::BufferStmt* Parser::parseBuffer() {
  ast::BufferStmt* buffer = _ast->createNode<ast::BufferStmt>();
  buffer->name = consume(TokenType::Identifier, "buffer name expected").lexeme();
  if (match(TokenType::Colon)) {
    consume(TokenType::Register, "register expected");
    consume(TokenType::LeftParen, "'(' expected");
    buffer->registerName = consume(TokenType::Identifier, "register name expected").lexeme();
    consume(TokenType::RightParen, "')' expected");
  }
  consume(TokenType::LeftBrace, "'{' expected for buffer");
  
  ast::Field* lastField = nullptr;
  while (!match(TokenType::RightBrace) && !isAtEnd()) {
    ast::Field* field = parseBufferField();
    if (field == nullptr) {
      throw ParseException(peekNext(), "field expected for buffer");
    }

    if (buffer->field == nullptr) {
      buffer->field = field;
    } else {
      lastField->next = field;
    }
    lastField = field;

    consume(TokenType::Semicolon, "';' expected for buffer field");
  }

  return buffer;
}

ast::Type* Parser::parseType(bool allowVoid, const char* exceptionMessage) {
  // We don't know if this is a type or a variable name yet, so start
  // recording the tokens in case we need to rewind.
  startRestorePoint();

  uint32_t flags = ast::TypeFlags::None;
  while (parseTypeModifier(flags) || parseInterpolationModifier(flags)) {}

  Token token = advance();

  if (token.type() == TokenType::Struct) {
    // We can discard the tokens we recorded because we know this is a type.
    discardRestorePoint();
    ast::StructStmt* structType = parseStruct();
    _ast->addStruct(structType);
    ast::Type* type = _ast->createNode<ast::Type>();
    type->flags = flags;
    type->baseType = ast::BaseType::Struct;
    type->name = structType->name;
    return type;
  }

  // If the token is an Identifier, it could be a user typedef type or a struct.
  if (token.type() == TokenType::Identifier) {
    if (_typedefs.find(token.lexeme()) != _typedefs.end()) {
      // We can discard the tokens we recorded because we know this is a type.
      discardRestorePoint();
      ast::Type* type = _ast->createNode<ast::Type>();
      type->flags = flags;
      type->baseType = ast::BaseType::UserDefined;
      type->name = token.lexeme();
      return type;
    }

    if (_structs.find(token.lexeme()) != _structs.end()) {
      // We can discard the tokens we recorded because we know this is a type.
      discardRestorePoint();
      ast::Type* type = _ast->createNode<ast::Type>();
      type->flags = flags;
      type->baseType = ast::BaseType::Struct;
      type->name = token.lexeme();
      return type;
    }

    // Roll back to the start of the type, because it wasn't a typedef or struct.
    restorePoint();

    if (exceptionMessage != nullptr) {
      // If a type was expected, throw an exception.
      throw ParseException(token, exceptionMessage);
    }

    // Otherwise the caller was just checking to see if it was a Type, so returning
    // null will indicate that it wasn't.
    return nullptr;
  }

  // Check to see if the token is a built-in type.
  ast::BaseType baseType = tokenTypeToBaseType(token.type());

  if (baseType == ast::BaseType::Undefined) {
    // If the token wasn't a built-in type, roll back to the start of the type.
    restorePoint();
    if (exceptionMessage != nullptr) {
      // If a type was expected, throw an exception.
      throw ParseException(token, exceptionMessage);
    }
    // Otherwise the caller was just checking to see if it was a Type, so returning
    // null will indicate that it wasn't.
    return nullptr;
  }

  if (baseType == ast::BaseType::Void) {
    // If it's a void type, check to see if void is allowed in this context.
    if (allowVoid) {
      // We can discard the tokens we recorded because we know this is an
      // accepted void type.
      discardRestorePoint();
      ast::Type* type = _ast->createNode<ast::Type>();
      type->flags = flags;
      type->baseType = baseType;
      return type;
    } else {
      // If void isn't allowed, roll back to the start of the type.
      restorePoint();
      if (exceptionMessage != nullptr) {
        // If a type was expected, throw an exception.
        throw ParseException(token, exceptionMessage);
      }
      // Otherwise the caller was just checking to see if it was a Type, so returning
      // null will indicate that it wasn't.
      return nullptr;
    }
  }
 
  // For template types like Samper<float> or RWStorageBuffer<float>, parse the template arguments.
  ast::TemplateArg* templateArg = nullptr;
  if (match(TokenType::Less)) {
    templateArg = _ast->createNode<ast::TemplateArg>();
    templateArg->value = parseType(false);
    if (match(TokenType::Comma)) {
      ast::TemplateArg* next = _ast->createNode<ast::TemplateArg>();
      next->value = parseSingularExpression();
      templateArg->next = next;
    }
    consume(TokenType::Greater, "'>' expected for template type");
  }

  // We can discard the recorded tokens we recorded because we know this is a type.
  discardRestorePoint();

  ast::Type* type = _ast->createNode<ast::Type>();
  type->flags = flags;
  type->baseType = baseType;
  type->templateArg = templateArg;

  return type;
}

bool Parser::parseTypeModifier(uint32_t& flags) {
  if (match(TokenType::Const)) {
    flags |= ast::TypeFlags::Const;
    return true;
  }
  
  if (match(TokenType::Static)) {
    flags |= ast::TypeFlags::Static;
    return true;
  }

  if (match(TokenType::Uniform)) {
    return true;
  }

  if (match(TokenType::Inline)) {
    return true;
  }

  if (match(TokenType::In)) {
    flags |= ast::TypeFlags::Input;
    return true;
  }

  if (match(TokenType::Out)) {
    flags |= ast::TypeFlags::Output;
    return true;
  }

  if (match(TokenType::Inout)) {
    flags |= ast::TypeFlags::Input;
    flags |= ast::TypeFlags::Output;
    return true;
  }

  if (match(TokenType::Unorm)) {
    flags |= ast::TypeFlags::Unorm;
    return true;
  }

  if (match(TokenType::Snorm)) {
    flags |= ast::TypeFlags::Snorm;
    return true;
  }

  return false;
}

bool Parser::parseInterpolationModifier(uint32_t& flags) {
  if (match(TokenType::Linear)) {
    flags |= ast::TypeFlags::Linear;
    return true;
  }

  if (match(TokenType::Centroid)) {
    flags |= ast::TypeFlags::Centroid;
    return true;
  }

  if (match(TokenType::Nointerpolation)) {
    flags |= ast::TypeFlags::Nointerpolation;
    return true;
  }

  if (match(TokenType::Noperspective)) {
    flags = flags | ast::TypeFlags::Noperspective;
    return true;
  }

  if (match(TokenType::Sample)) {
    flags = flags | ast::TypeFlags::Sample;
    return true;
  }

  return false;
}

ast::Expression* Parser::parseLogicalOrExpression() {
  ast::Expression* expr = parseLogicalAndExpression();
  while (match(TokenType::PipePipe)) {
    ast::BinaryExpr* op = _ast->createNode<ast::BinaryExpr>();
    op->op = ast::Operator::OrOr;
    op->left = expr;
    op->right = parseLogicalAndExpression();
    expr = op;
  }
  return expr;
}

ast::Expression* Parser::parseLogicalAndExpression() {
  ast::Expression* expr = parseInclusiveOrExpression();
  while (match(TokenType::AmpersandAmpersand)) {
    ast::BinaryExpr* op = _ast->createNode<ast::BinaryExpr>();
    op->op = ast::Operator::AndAnd;
    op->left = expr;
    op->right = parseInclusiveOrExpression();
    expr = op;
  }
  return expr;
}

ast::Expression* Parser::parseInclusiveOrExpression() {
  ast::Expression* expr = parseExclusiveOrExpression();
  while (match(TokenType::Pipe)) {
    ast::BinaryExpr* op = _ast->createNode<ast::BinaryExpr>();
    op->op = ast::Operator::Or;
    op->left = expr;
    op->right = parseExclusiveOrExpression();
    expr = op;
  }
  return expr;
}

ast::Expression* Parser::parseExclusiveOrExpression() {
  ast::Expression* expr = parseAndExpression();
  while (match(TokenType::Caret)) {
    ast::BinaryExpr* op = _ast->createNode<ast::BinaryExpr>();
    op->op = ast::Operator::Xor;
    op->left = expr;
    op->right = parseAndExpression();
    expr = op;
  }
  return expr;
}

ast::Expression* Parser::parseAndExpression() {
  ast::Expression* expr = parseEqualityExpression();
  while (match(TokenType::Ampersand)) {
    ast::BinaryExpr* op = _ast->createNode<ast::BinaryExpr>();
    op->op = ast::Operator::And;
    op->left = expr;
    op->right = parseEqualityExpression();
    expr = op;
  }
  return expr;
}

ast::Expression* Parser::parseEqualityExpression() {
  ast::Expression* expr = parseRelationalExpression();
  while (check(TokenType::EqualEqual) || check(TokenType::BangEqual)) {
    Token tk = advance();
    ast::BinaryExpr* op = _ast->createNode<ast::BinaryExpr>();
    op->op = tk.type() == TokenType::EqualEqual
        ? ast::Operator::EqualEqual : ast::Operator::NotEqual;
    op->left = expr;
    op->right = parseRelationalExpression();
    expr = op;
  }
  return expr;
}

ast::Expression* Parser::parseRelationalExpression() {
  ast::Expression* expr = parseShiftExpression();
  while (check(TokenType::Greater) || check(TokenType::Less) ||
        check(TokenType::GreaterEqual) || check(TokenType::LessEqual)) {
    Token tk = advance();
    ast::BinaryExpr* op = _ast->createNode<ast::BinaryExpr>();
    op->op = tk.type() == TokenType::Greater ? ast::Operator::Greater :
             tk.type() == TokenType::Less ? ast::Operator::Less :
             tk.type() == TokenType::GreaterEqual ? ast::Operator::GreaterEqual :
             ast::Operator::LessEqual;
    op->left = expr;
    op->right = parseShiftExpression();
    expr = op;
  }
  return expr;
}

ast::Expression* Parser::parseShiftExpression() {
  ast::Expression* expr = parseAdditiveExpression();
  while (check(TokenType::GreaterGreater) || check(TokenType::LessLess)) {
    Token tk = advance();
    ast::BinaryExpr* op = _ast->createNode<ast::BinaryExpr>();
    op->op = tk.type() == TokenType::GreaterGreater ? ast::Operator::RightShift :
             ast::Operator::LeftShift;
    op->left = expr;
    op->right = parseAdditiveExpression();
    expr = op;
  }
  return expr;
}

ast::Expression* Parser::parseAdditiveExpression() {
  ast::Expression* expr = parseMultiplicativeExpression();
  while (check(TokenType::Plus) || check(TokenType::Minus)) {
    Token tk = advance();
    ast::BinaryExpr* op = _ast->createNode<ast::BinaryExpr>();
    op->op = tk.type() == TokenType::Plus ? ast::Operator::Add : ast::Operator::Subtract;
    op->left = expr;
    op->right = parseMultiplicativeExpression();
    expr = op;
  }
  return expr;
}

ast::Expression* Parser::parseMultiplicativeExpression() {
  ast::Expression* expr = parsePrefixExpression();
  while (check(TokenType::Star) || check(TokenType::Slash) || check(TokenType::Percent) ||
      check(TokenType::PlusPlus) || check(TokenType::MinusMinus)) {
    Token tk = advance();
    ast::BinaryExpr* op = _ast->createNode<ast::BinaryExpr>();
    op->op = tk.type() == TokenType::Star ? ast::Operator::Multiply :
             tk.type() == TokenType::Slash ? ast::Operator::Divide : ast::Operator::Modulo;
    op->left = expr;
    op->right = parsePrefixExpression();
    expr = op;
  }
  return expr;
}

ast::Expression* Parser::parsePrefixExpression() {
  if (check(TokenType::Minus) || check(TokenType::Bang) || check(TokenType::Tilde) ||
      check(TokenType::PlusPlus) || check(TokenType::MinusMinus)) {
    Token tk = advance();
    ast::PrefixExpr* op = _ast->createNode<ast::PrefixExpr>();
    op->op = tk.type() == TokenType::Minus ? ast::Operator::Subtract :
             tk.type() == TokenType::Bang ? ast::Operator::Not :
             tk.type() == TokenType::Tilde ? ast::Operator::BitNot :
             tk.type() == TokenType::PlusPlus ? ast::Operator::PlusPlus :
             ast::Operator::MinusMinus;
    op->expression = parsePrefixExpression();
    return op;
  }
  return parseSingularExpression();
}

ast::Expression* Parser::parseSingularExpression() {
  ast::Expression* expr = parsePrimaryExpression();
  expr = parsePostfixExpression(expr);

  if (match(TokenType::Question)) {
    // Ternary conditional expression (a ? b : c)
    ast::TernaryExpr* op = _ast->createNode<ast::TernaryExpr>();
    op->condition = expr;
    op->trueExpr = parseExpression();
    consume(TokenType::Colon, "Expected ':' after ternary expression");
    op->falseExpr = parseExpression();
    expr = op;
  }

  return expr;
}

ast::Expression* Parser::parsePostfixExpression(ast::Expression* expr) {
  // ++, --, [], .
  if (check(TokenType::PlusPlus) || check(TokenType::MinusMinus)) {
    Token tk = advance();
    ast::IncrementExpr* op = _ast->createNode<ast::IncrementExpr>();
    op->op = tk.type() == TokenType::PlusPlus ? ast::Operator::PlusPlus : ast::Operator::MinusMinus;
    op->variable = expr;
    return parsePostfixExpression(op);
  }

  if (match(TokenType::LeftBracket)) {
    ast::Expression* index = parseExpression();
    consume(TokenType::RightBracket, "Expected ']' after expression");
    ast::ArrayExpr* array = _ast->createNode<ast::ArrayExpr>();
    array->array = expr;
    array->index = index;
    return parsePostfixExpression(array);
  }

  if (match(TokenType::Dot)) {
    ast::MemberExpr* member = _ast->createNode<ast::MemberExpr>();
    member->object = expr;
    member->member = parseSingularExpression();   
    return parsePostfixExpression(member);
  }

  return expr;
}

ast::Expression* Parser::parsePrimaryExpression() {
  if (check(TokenType::Identifier)) {
    // An identifier is either a variable or a function call
    Token name = advance();
    if (check(TokenType::LeftParen)) {
      ast::CallExpr* expr = _ast->createNode<ast::CallExpr>();
      expr->name = name.lexeme();
      expr->arguments = parseArgumentList();
      return expr;
    }

    ast::VariableExpr* expr = _ast->createNode<ast::VariableExpr>();
    expr->name = name.lexeme();
    return expr;
  }

  if (check(TokenType::IntLiteral) || check(TokenType::FloatLiteral) || check(TokenType::True) ||
      check(TokenType::False)) {
    Token tk = advance();
    ast::LiteralExpr* expr = _ast->createNode<ast::LiteralExpr>();
    expr->type = tokenTypeToBaseType(tk.type());
    expr->value = tk.lexeme();
    return expr;
  }

  if (check(TokenType::LeftParen)) {
    // Either a parenthesized expression or a cast expression
    startRestorePoint();
    advance(); // consume '('
    if (isType(peekNext())) {
      advance();
      if (check(TokenType::RightParen)) {
        // (Type) is a cast expression
        restorePoint();

        advance();  // consume '('
        ast::Type* type = parseType(false, "Invalid type");
        consume(TokenType::RightParen, "Expected ')' after type");

        ast::Expression* valueExpr = parseSingularExpression();
        ast::CastExpr* expr = _ast->createNode<ast::CastExpr>();
        expr->type = type;
        expr->value = valueExpr;
        return expr;
      }
    }
    // parenthesized expression (x)
    restorePoint();
    return parseParenthesizedExpression();
  }

  Token tk = peekNext();
  // float(x) is a cast expression
  if (isType(tk)) {
    ast::CastExpr* expr = _ast->createNode<ast::CastExpr>();
    expr->type = parseType(false, "Invalid type");
    consume(TokenType::LeftParen, "Expected '(' after type");
    expr->value = parseExpressionList();
    consume(TokenType::RightParen, "Expected ')' after expression");
    return expr;
  }

  // (float)x or (float)(x) are cast expressions
  if (tk.type() == TokenType::LeftParen) {
    advance();
    ast::Type* type = parseType(false, "Invalid type");
    consume(TokenType::RightParen, "Expected ')' after type");

    ast::Expression* valueExpr = nullptr;
    if (peekNext().type() == TokenType::LeftParen) {
      // (float)(x) is a cast expression
      valueExpr = parseParenthesizedExpression();
    } else {
      // (float)x is a cast expression
      valueExpr = parseSingularExpression();
    }

    ast::CastExpr* expr = _ast->createNode<ast::CastExpr>();
    expr->type = type;
    expr->value = valueExpr;

    return expr;
  }

  return nullptr;
}

ast::Expression* Parser::parseParenthesizedExpression() {
  consume(TokenType::LeftParen, "Expected '(' after expression");
  ast::Expression* expr = parseLogicalOrExpression();
  consume(TokenType::RightParen, "Expected ')' after expression");
  return expr;
}

ast::Expression* Parser::parseArgumentList() {
  consume(TokenType::LeftParen, "Expected '(' after function name");
  ast::Expression* firstExpr = parseLogicalOrExpression();
  ast::Expression* lastExpr = firstExpr;
  while (match(TokenType::Comma)) {
    if (check(TokenType::RightParen)) {
      // trailing comma
      break;
    }
    ast::Expression* expr = parseLogicalOrExpression();
    lastExpr->next = expr;
    lastExpr = expr;
  }
  consume(TokenType::RightParen, "Expected ')' after argument list");
  return firstExpr;
}

ast::FunctionStmt* Parser::parseFunctionStmt(ast::Type* returnType, const std::string_view& name) {
  ast::FunctionStmt* func = _ast->createNode<ast::FunctionStmt>();
  func->returnType = returnType;
  func->name = name;
  func->parameters = parseParameterList();

  if (match(TokenType::Colon)) {
    func->semantic = advance().lexeme();
  }

  func->body = parseBlock();
  return func;
}

ast::Parameter* Parser::parseParameterList() {
  consume(TokenType::LeftParen, "Expected '(' after function name");
  if (match(TokenType::RightParen)) {
    // Empty parameter list
    return nullptr;
  }

  ast::Parameter* firstParam = parseParameter();
  ast::Parameter* lastParam = firstParam;
  while (match(TokenType::Comma)) {
    if (check(TokenType::RightParen)) {
      break;
    }
    ast::Parameter* param = parseParameter();
    lastParam->next = param;
    lastParam = param;
  }

  consume(TokenType::RightParen, "Expected ')' after parameter list");
  return firstParam;
}

ast::Parameter* Parser::parseParameter() {
  ast::Parameter* param = _ast->createNode<ast::Parameter>();
  param->type = parseType(false, "Expected parameter type");
  param->name = consume(TokenType::Identifier, "Expected parameter name").lexeme();

  if (match(TokenType::LeftBracket)) {
    // Array parameter (int a[*])
    param->isArray = true;
    param->arraySize = parseArraySize();
  }

  if (match(TokenType::Colon)) {
    param->semantic = advance().lexeme();
  }

  if (match(TokenType::Equal)) {
    param->initializer = parseExpression();
  }
  return param;
}

ast::VariableStmt* Parser::parseVariableStmt(ast::Type* type, const std::string_view& name,
                                           ast::Attribute* attributes) {
  ast::VariableStmt* var = _ast->createNode<ast::VariableStmt>();
  var->type = type;
  var->name = name;
  var->attributes = attributes;

  ast::Expression* lastArraySize = nullptr;
  // Parse array dimensions (int a[1][2])
  while (match(TokenType::LeftBracket)) {
    var->isArray = true;
    if (var->arraySize == nullptr) {
      var->arraySize = parseArraySize();
      lastArraySize = var->arraySize;
    } else {
      lastArraySize->next = parseArraySize();
      lastArraySize = (ast::LiteralExpr*)lastArraySize->next;
    }
  }

  // Parse variable initializer (int a = 1)
  if (match(TokenType::Equal)) {
    var->initializer = parseAssignmentExpression(var->type);
  }

  _variables[var->name] = var;

  ast::VariableStmt* firstVar = var;
  // Parse multiple variable declarations (a = b, c = d)
  while (match(TokenType::Comma) && !isAtEnd()) {
    std::string_view name = consume(TokenType::Identifier, "Expected variable name").lexeme();
    ast::VariableStmt* next = _ast->createNode<ast::VariableStmt>();
    next->type = type;
    next->name = name;
    next->attributes = attributes;

    if (match(TokenType::LeftBracket)) {
      next->isArray = true;
      next->arraySize = parseArraySize();
    }

    if (match(TokenType::Equal)) {
      next->initializer = parseLogicalOrExpression();
    }

    _variables[next->name] = next;

    next->attributes = attributes;
    var->next = next;
    var = next;
  }

  return firstVar;
}

ast::Expression* Parser::parseArraySize() {
  ast::Expression* firstSize = nullptr;
  // Array declaration (int a[10])
  Token next = peekNext();
  if (next.type() == TokenType::Identifier) {
    advance();
    auto var = _variables.find(next.lexeme());
    if (var == _variables.end()) {
      throw ParseException(next, "Unknown variable");
    }
    ast::VariableStmt* varStmt = var->second;
    if (!varStmt->type->isConst() || varStmt->type->baseType != ast::BaseType::Int) {
      throw ParseException(next, "Expected const int");
    }
    if (varStmt->initializer == nullptr ||
        varStmt->initializer->nodeType != ast::NodeType::LiteralExpr) {
      throw ParseException(next, "Expected const int");
    }
    ast::LiteralExpr* size = (ast::LiteralExpr*)varStmt->initializer;
    /*if (size->type != ast::BaseType::Int) {
      throw ParseException(next, "Expected const int");
    }*/

    firstSize = size;
  } else if (next.type() == TokenType::IntLiteral) {
    ast::Expression* size = parseExpression();
    firstSize = size;

    /*Token count = advance();
    ast::LiteralExpr* size = _ast->createNode<ast::LiteralExpr>();
    size->type = ast::BaseType::Int;
    size->value = count.lexeme();
    firstSize = size;*/

    // This isn't needed if HLSL doesn't support multi-dimensional arrays declared as [10, 20].
    /*while (match(TokenType::Comma) && !isAtEnd()) {
      Token count = consume(TokenType::IntLiteral, "Expected array size");
      ast::LiteralExpr* nextSize = _ast->createNode<ast::LiteralExpr>();
      nextSize->value = count.lexeme();
      size->next = nextSize;
      size = nextSize;
    }*/
  }

  consume(TokenType::RightBracket, "Expected ']' for array declaration");
  return firstSize;
}

// Parse a block of statements enclosed in braces
ast::Block* Parser::parseBlock() {
  ast::Block* block = _ast->createNode<ast::Block>();

  if (check(TokenType::Semicolon)) {
    // We have a function forward declaration.  Just eat the
    //   semicolon and return
    consume(TokenType::Semicolon);
    return block;
  }

  consume(TokenType::LeftBrace, "Expected '{' before block");

  // Empty block {}
  if (match(TokenType::RightBrace)) {
    return block;
  }

  block->statements = parseStatement();
  if (block->statements == nullptr) {
    consume(TokenType::RightBrace, "Expected '}' after block");
    return block;
  }

  ast::Statement* lastStmt = block->statements;
  while (!check(TokenType::RightBrace) && !isAtEnd()) {
    ast::Statement* stmt = parseStatement();
    if (stmt == nullptr) {
      throw ParseException(peekNext(), "Expected statement");
    }
    if (stmt->nodeType == ast::NodeType::EmptyStmt) {
      continue;
    }
    lastStmt->next = stmt;
    lastStmt = stmt;
  }

  consume(TokenType::RightBrace, "Expected '}' after block");
  return block;
}

ast::Statement* Parser::parseStatement(bool expectSemicolon) {
  while (match(TokenType::Semicolon)) {
    // Ignore empty statements
  }

  if (check(TokenType::RightBrace)) {
    return &ast::EmptyStatement::instance;
  }

  // Attributes are really only for top-level statements, but checking for
  // them in all cases until we pass in something to let us know this is
  // a top-level statement.
  ast::Attribute* attributes = parseAttributes();

  if (match(TokenType::If)) {
    ast::IfStmt* stmt = parseIfStmt();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::Switch)) {
    ast::SwitchStmt* stmt = parseSwitchStmt();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::For)) {
    ast::ForStmt* stmt = parseForStmt();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::Do)) {
    ast::DoWhileStmt* stmt = parseDoWhileStmt();
    stmt->attributes = attributes;
    if (expectSemicolon) {
      consume(TokenType::Semicolon, "Expected ';' after return value");
    }
    return stmt;
  }

  if (match(TokenType::While)) {
    ast::WhileStmt* stmt = parseWhileStmt();
    stmt->attributes = attributes;
    return stmt;
  }

  if (check(TokenType::LeftBrace)) {
    ast::Statement* stmt = parseBlock();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::Return)) {
    ast::ReturnStmt* stmt = _ast->createNode<ast::ReturnStmt>();
    stmt->attributes = attributes;

    if (check(TokenType::Semicolon)) {
      return stmt;
    }

    stmt->value = parseExpression();

    Token assignmentOp = peekNext();
    if (isAssignmentOperator(assignmentOp.type())) {
      advance(); // consume the assignment operator
      ast::Expression* value = parseExpression();
      if (value == nullptr) {
        throw ParseException(assignmentOp, "Expected expression after assignment operator");
      }
      ast::AssignmentExpr* assign = _ast->createNode<ast::AssignmentExpr>();
      assign->op = tokenTypeToAssignmentOperatator(assignmentOp.type());
      assign->variable = stmt->value;
      assign->value = value;
      stmt->value = assign;
    }

    if (expectSemicolon) {
      consume(TokenType::Semicolon, "Expected ';' after return value");
    }

    return stmt;
  }

  if (match(TokenType::Break)) {
    ast::BreakStmt* stmt = _ast->createNode<ast::BreakStmt>();
    stmt->attributes = attributes;
    if (expectSemicolon) {
      consume(TokenType::Semicolon, "Expected ';' after 'break'");
    }
    return stmt;
  }

  if (match(TokenType::Continue)) {
    ast::ContinueStmt* stmt = _ast->createNode<ast::ContinueStmt>();
    stmt->attributes = attributes;
    if (expectSemicolon) {
      consume(TokenType::Semicolon, "Expected ';' after 'continue'");
    }
    return stmt;
  }

  if (match(TokenType::Discard)) {
    ast::DiscardStmt* stmt = _ast->createNode<ast::DiscardStmt>();
    stmt->attributes = attributes;
    if (expectSemicolon) {
      consume(TokenType::Semicolon, "Expected ';' after 'discard'");
    }
    return stmt;
  }

  ast::Statement* stmt = nullptr;

  if (check(TokenType::Identifier)) {
    Token name = advance();
    if (check(TokenType::LeftParen)) {
      ast::CallStmt* call = _ast->createNode<ast::CallStmt>();
      call->name = name.lexeme();
      call->arguments = parseArgumentList();
      call->attributes = attributes;
      if (expectSemicolon) {
        consume(TokenType::Semicolon, "Expected ';' after statment");
      }
      return call;
    }

    pushBack(name);
  }

  ast::Type* type = parseType(false);
  if (type != nullptr) {
    const std::string_view name = consume(TokenType::Identifier, "Expected variable name").lexeme();
    stmt = parseVariableStmt(type, name, attributes);   
    if (match(TokenType::Comma)) {
      ast::Statement* next = parseStatement(expectSemicolon);
      if (next != nullptr && next->nodeType != ast::NodeType::EmptyStmt) {
        stmt->next = next;
      }
    } else if (expectSemicolon) {
      consume(TokenType::Semicolon, "Expected ';' after variable declaration");
    }
    return stmt;
  }

  if (check(TokenType::Underscore) || check(TokenType::Identifier)) {
    startRestorePoint();
    // Assignment statement or method call (a.b())
    const bool isUnderscore = match(TokenType::Underscore);

    ast::Expression* var = nullptr;

    if (!isUnderscore) {
      var = parsePrefixExpression();
    }

    if (!isUnderscore && var == nullptr) {
      throw ParseException(peekNext(), "Expected variable name");
    }

    Token tk = advance();

    if (tk.type() == TokenType::Semicolon) {
      // We're done here, the "assignment" was actually a method call.
      ast::ExpressionStmt* stmt = _ast->createNode<ast::ExpressionStmt>();
      stmt->expression = var;
      stmt->attributes = attributes;
      discardRestorePoint();
      return stmt;
    }

    ast::Operator op = tokenTypeToAssignmentOperatator(tk.type());
    if (op != ast::Operator::Undefined) {
      ast::AssignmentStmt* stmt = _ast->createNode<ast::AssignmentStmt>();
      stmt->variable = var;
      stmt->op = op;
      stmt->value = parseAssignmentExpression(type);

      if (match(TokenType::Comma)) {
        ast::Statement* next = parseStatement(expectSemicolon);
        if (next != nullptr && next->nodeType != ast::NodeType::EmptyStmt) {
          stmt->next = next;
        }
      } else if (expectSemicolon) {
        consume(TokenType::Semicolon, "Expected ';' after assignment");
      }

      stmt->attributes = attributes;
      discardRestorePoint();
      return stmt;
    }

    restorePoint();
  }

  try {
    ast::Expression* expr = parseExpression();
    if (expr != nullptr) {
      ast::ExpressionStmt* exprStmt = _ast->createNode<ast::ExpressionStmt>();
      exprStmt->expression = expr;
      stmt = exprStmt;
    }
  } catch (ParseException& e) {
  }

  if (stmt != nullptr) {
    
    if (match(TokenType::Comma)) {
      ast::Statement* next = parseStatement(expectSemicolon);
      if (next != nullptr && next->nodeType != ast::NodeType::EmptyStmt) {
        stmt->next = next;
      }
    } else if (expectSemicolon) {
      consume(TokenType::Semicolon, "Expected ';' after expression");
    }
    stmt->attributes = attributes;
    return stmt;
  }

  return nullptr;
}

ast::IfStmt* Parser::parseIfStmt() {
  ast::IfStmt* stmt = _ast->createNode<ast::IfStmt>();

  consume(TokenType::LeftParen, "Expected '(' after 'if'");

  stmt->condition = parseExpression();

  consume(TokenType::RightParen, "Expected ')' after 'if' condition");

  stmt->body = parseStatement();

  if (match(TokenType::Else)) {
    stmt->elseBody = parseStatement();
  }

  return stmt;
}

ast::SwitchStmt* Parser::parseSwitchStmt() {
  ast::SwitchStmt* switchStmt = _ast->createNode<ast::SwitchStmt>();
  
  consume(TokenType::LeftParen, "Expected '(' after 'switch'");
  switchStmt->condition = parseExpression();
  consume(TokenType::RightParen, "Expected ')' after 'switch' condition");
  
  consume(TokenType::LeftBrace, "Expected '{' after 'switch' condition");
  
  ast::SwitchCase* lastCase = nullptr;
  while (!match(TokenType::RightBrace) && !isAtEnd()) {
    ast::SwitchCase* caseStmt = _ast->createNode<ast::SwitchCase>();

    if (match(TokenType::Case)) {      
      caseStmt->isDefault = false;
      caseStmt->condition = parseExpression();
    } else if (match(TokenType::Default)) {
      caseStmt->isDefault = true;
    } else {
      throw ParseException(peekNext(), "Expected 'case' or 'default' in switch statement");
    }

    if (switchStmt->cases == nullptr) {
      lastCase = caseStmt;
      switchStmt->cases = caseStmt;
    } else {
      lastCase->next = caseStmt;
    }

    consume(TokenType::Colon, "Expected ':' after 'case' or 'default'");

    ast::Statement* firstStatement = nullptr;
    ast::Statement* lastStatement = nullptr;

    Token next = peekNext();
    while (next.type() != TokenType::Case && next.type() != TokenType::Default && next.type() != TokenType::RightBrace) {
      ast::Statement* caseBodyStmt = parseStatement();
      if (caseBodyStmt != nullptr) {
        if (firstStatement == nullptr) {
          firstStatement = caseBodyStmt;
        } else {
          lastStatement->next = caseBodyStmt;
        }
        lastStatement = caseBodyStmt;
      }
      if (caseBodyStmt->nodeType == ast::NodeType::BreakStmt || caseBodyStmt->nodeType == ast::NodeType::ReturnStmt) {
        break;
      }
      next = peekNext();
    }

    caseStmt->body = firstStatement;
  }
  return switchStmt;
}

ast::ForStmt* Parser::parseForStmt() {
  ast::ForStmt* stmt = _ast->createNode<ast::ForStmt>();

  consume(TokenType::LeftParen, "Expected '(' after 'for'");
  if (!check(TokenType::Semicolon)) {
    stmt->initializer = parseStatement(false);
  }
  consume(TokenType::Semicolon, "Expected ';' after 'for' initializer");

  if (!check(TokenType::Semicolon)) {
    stmt->condition = parseExpression();
  }
  consume(TokenType::Semicolon, "Expected ';' after 'for' condition");

  if (!check(TokenType::RightParen)) {
    stmt->increment = parseStatement(false);
    ast::Statement* next = stmt->increment;
    while (match(TokenType::Comma)) {
      next->next = parseStatement(false);
      next = next->next;
    }
  }
  consume(TokenType::RightParen, "Expected ')' after 'for' increment");

  stmt->body = parseStatement();

  return stmt;
}

ast::DoWhileStmt* Parser::parseDoWhileStmt() {
  ast::DoWhileStmt* stmt = _ast->createNode<ast::DoWhileStmt>();

  stmt->body = parseBlock();

  consume(TokenType::While, "Expected 'while' after 'do' block");
  consume(TokenType::LeftParen, "Expected '(' after 'while'");
  stmt->condition = parseExpression();
  consume(TokenType::RightParen, "Expected ')' after 'while' condition");

  return stmt;
}

ast::WhileStmt* Parser::parseWhileStmt() {
  ast::WhileStmt* stmt = _ast->createNode<ast::WhileStmt>();
  consume(TokenType::LeftParen, "Expected '(' after 'while'");
  stmt->condition = parseExpression();
  consume(TokenType::RightParen, "Expected ')' after 'while' condition");
  stmt->body = parseStatement();
  return stmt;
}

} // namespace hlsl
} // namespace reader
