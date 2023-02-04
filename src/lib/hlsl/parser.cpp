#include "parser.h"

#include "../util/string_util.h"
#include "ast/type_flags.h"
#include "parser/effect_state.h"
#include "parse_exception.h"

namespace hlsl {

Parser::Parser(const std::string_view& source)
  : _scanner(source) {
}

Ast* Parser::parse() {
  _ast = new Ast();

  AstRoot* root = _ast->root();

  AstStatement* lastStatement = nullptr;
  while (!isAtEnd()) {
    AstStatement* statement = nullptr;
    try {
      statement = parseTopLevelStatement();
    } catch (ParseException e) {
      std::cerr << "Error: "  << e.message << std::endl;
      //std::cerr << "  Filename: " << _scanner.filename() << std::endl;
      //std::cerr << "  Line: " << _scanner.line() << std::endl;
      std::cerr << "  Line: " << _scanner.absoluteLine() << std::endl;
      delete _ast;
      _ast = nullptr;
      return nullptr;
    }
    if (statement == nullptr) {
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
    if (_restorePoint > 0) {
      _restore.push_back(t);
    }
    return t;
  }

  if (_restorePoint > 0) {
    Token t = _scanner.scanNext();
    _restore.push_back(t);
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

AstStatement* Parser::parseTopLevelStatement() {
  // Discard any solo semicolons
  while (match(TokenType::Semicolon) && !isAtEnd()) {}

  AstAttribute* attributes = parseAttributes();

  if (match(TokenType::Struct)) {
    AstStructStmt* node = parseStruct();
    if (node != nullptr) {
      node->attributes = attributes;
    }
    return node;
  }

  if (match(TokenType::Cbuffer) || match(TokenType::Tbuffer)) {
    AstBufferStmt* node = parseBuffer();
    if (node != nullptr) {
      node->attributes = attributes;
    }
    return node;
  }

  if (match(TokenType::Typedef)) {
    AstTypedefStmt* node = parseTypedef();
    if (node != nullptr) {
      node->attributes = attributes;
    }
    consume(TokenType::Semicolon, "Expected ';' after typedef");
    return node;
  }

  // Global Declaration
  AstType* type = parseType(true);
  if (type != nullptr) {
    // Either a variable or function declaration.
    Token identifier = consume(TokenType::Identifier, "identifier expected");

    // function declaration
    if (check(TokenType::LeftParen)) {
      AstFunctionStmt* func = parseFunctionStmt(type, identifier.lexeme());
      if (func != nullptr) {
        func->attributes = attributes;
      }
      return func;
    }

    // variable declaration
    AstVariableStmt* var = parseVariableStmt(type, identifier.lexeme());
    if (var != nullptr) {
      var->attributes = attributes;
    }
    return var;
  }

  return nullptr;
}

AstTypedefStmt* Parser::parseTypedef() {
  AstTypedefStmt* node = _ast->createNode<AstTypedefStmt>();
  node->type = parseType(true, "typedef type expected");
  node->name = advance().lexeme();
  _typedefs[node->name] = node;
  return node;
}

AstStructStmt* Parser::parseStruct() {
  Token name = consume(TokenType::Identifier, "struct name expected.");
  consume(TokenType::LeftBrace, "'{' expected for struct");

  AstStructField* firstField = nullptr;
  AstStructField* lastField = nullptr;
  while (!check(TokenType::RightBrace) && !isAtEnd()) {
    AstStructField* field = parseStructField();
    if (firstField == nullptr) {
      firstField = field;
    }
    if (lastField != nullptr) {
      lastField->next = field;
    }
    lastField = field;
  }

  consume(TokenType::RightBrace, "'}' expected for struct");
  consume(TokenType::Semicolon, "';' expected for struct");

  AstStructStmt* s = _ast->createNode<AstStructStmt>();
  s->name = name.lexeme();
  s->fields = firstField;
  return s;
}

AstStructField* Parser::parseStructField() {
  AstStructField* field = _ast->createNode<AstStructField>();

  Token tk = peekNext();
  if (isInterpolationModifier(tk.type())) {
    field->interpolation = tokenTypeToInterpolationModifier(tk.type());
    advance();
  }

  field->type = parseType(false, "struct field type expected");
  field->name = advance().lexeme();
  consume(TokenType::Semicolon, "';' expected for struct field");

  return field;
}

AstAttribute* Parser::parseAttributes() {
  // Parse attributes in the forms:
  // [A] statement
  // [A, B] statement
  // [A][B] statement
  // Where: A, B are attributes in the form of:
  // A
  // A(a[,b]*) where a and b are expressions
  AstAttribute* firstAttribute = nullptr;
  AstAttribute* lastAttribute = nullptr;;

  if (!match(TokenType::LeftBracket)) {
    return firstAttribute;
  }

  while (!isAtEnd()) {
    AstAttribute* attribute = _ast->createNode<AstAttribute>();
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
      AstExpression* firstArg = nullptr;
      AstExpression* lastArg = nullptr;
      while (!isAtEnd() && !match(TokenType::RightParen)) {
        AstExpression* arg = parseExpression();
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

AstExpression* Parser::parseExpression() {
  return parseLogicalOrExpression();
}

AstExpression* Parser::parseExpressionList() {
  AstExpression* firstExpression = parseExpression();
  if (firstExpression == nullptr) {
    return nullptr;
  }
  AstExpression* lastExpression = firstExpression;

  while (!isAtEnd() && match(TokenType::Comma)) {
    AstExpression* expression = parseExpression();
    if (expression == nullptr) {
      throw ParseException(peekNext(), "expression expected for expression list");
      break;
    }
    lastExpression->next = expression;
    lastExpression = expression;
  }

  return firstExpression;
}

AstBufferField* Parser::parseBufferField() {
  AstType* type = parseType(false, "buffer field type expected");

  AstBufferField* lastDecl = nullptr;
  AstBufferField* firstDecl = nullptr;

  do {
    AstBufferField* decl = _ast->createNode<AstBufferField>();
    if (firstDecl == nullptr) {
      firstDecl = decl;
    }
    if (lastDecl != nullptr) {
      lastDecl->next = decl;
    }
    lastDecl = decl;

    decl->name = consume(TokenType::Identifier, "identifier expected for declaration").lexeme();

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
  } while (match(TokenType::Comma));

  return firstDecl;
}

AstSamplerState* Parser::parseSamplerState() {
  consume(TokenType::SamplerState, "sampler state expected");
  consume(TokenType::LeftBrace, "'{' expected for sampler state");

  AstSamplerState* samplerState = _ast->createNode<AstSamplerState>();

  AstStateAssignment* lastStateAssignment = nullptr;
  while (!match(TokenType::RightBrace) && !isAtEnd()) {
    AstStateAssignment* stateAssignment = parseStateAssignment(true, true);
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

AstStateAssignment* Parser::parseStateAssignment(bool isSamplerState, bool isPipelineState) {
  AstStateAssignment* stateAssignment = _ast->createNode<AstStateAssignment>();

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
          mask |= toInt(tk.lexeme());
        } else if (tk.type() == TokenType::Identifier) {
          // color mask value
          const EffectStateValue* stateValue = colorMaskValues;
          while (stateValue->name != nullptr) {
            if (stringEqualCaseInsensitive(tk.lexeme(), stateValue->name)) {
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
      stateAssignment->intValue = toInt(tk.lexeme());
    } else if (expectsFloat) {
      Token tk = consume(TokenType::FloatLiteral, "float expected for state assignment");
      stateAssignment->floatValue = toFloat(tk.lexeme());
    } else if (expectsBoolean) {
      const Token& tk = advance();
      if (tk.type() == TokenType::Identifier) {
        const EffectStateValue* value = getEffectStateValue(tk.lexeme(), state);
        if (value != nullptr) {
          stateAssignment->intValue = value->value;
        }
      } else if (tk.type() == TokenType::IntLiteral) {
        stateAssignment->intValue = toInt(tk.lexeme());
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

AstBufferStmt* Parser::parseBuffer() {
  AstBufferStmt* buffer = _ast->createNode<AstBufferStmt>();
  buffer->name = consume(TokenType::Identifier, "buffer name expected").lexeme();
  if (match(TokenType::Colon)) {
    consume(TokenType::Register, "register expected");
    consume(TokenType::LeftParen, "'(' expected");
    buffer->registerName = consume(TokenType::Identifier, "register name expected").lexeme();
    consume(TokenType::RightParen, "')' expected");
  }
  consume(TokenType::LeftBrace, "'{' expected for buffer");
  
  AstBufferField* lastField = nullptr;
  while (!match(TokenType::RightBrace) && !isAtEnd()) {
    AstBufferField* field = parseBufferField();
    if (field == nullptr) {
      throw ParseException(peekNext(), "field expected for buffer");
    }

    field->buffer = buffer;
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

AstType* Parser::parseType(bool allowVoid, const char* exceptionMessage) {
  startRestorePoint();

  uint32_t flags = TypeFlags::None;
  while (parseTypeModifier(flags) || parseInterpolationModifier(flags)) {}

  Token token = advance();

  if (token.type() == TokenType::Identifier) {
    if (_typedefs.find(token.lexeme()) != _typedefs.end()) {
      discardRestorePoint();
      AstType* type = _ast->createNode<AstType>();
      type->flags = flags;
      type->baseType = BaseType::UserDefined;
      type->name = token.lexeme();
      return type;
    }

    restore();
    if (exceptionMessage != nullptr) {
      throw ParseException(token, exceptionMessage);
    }
    return nullptr;
  }

  BaseType baseType = tokenTypeToBaseType(token.type());

  if (baseType == BaseType::Undefined) {
    restore();
    if (exceptionMessage != nullptr) {
      throw ParseException(token, exceptionMessage);
    }
    return nullptr;
  }

  if (baseType == BaseType::Void) {
    if (allowVoid) {
      discardRestorePoint();
      AstType* type = _ast->createNode<AstType>();
      type->flags = flags;
      type->baseType = baseType;
      return type;
    } else {
      restore();
      if (exceptionMessage != nullptr) {
      throw ParseException(token, exceptionMessage);
    }
      return nullptr;
    }
  }
 
  SamplerType sType = SamplerType::Undefined;
  if (isSamplerBaseType(baseType)) {
    if (match(TokenType::Less)) {
      Token samplerType = advance();
      sType = tokenTypeToSamplerType(samplerType.type());
      if (sType == SamplerType::Undefined) {
        restore();
        if (exceptionMessage != nullptr) {
          throw ParseException(token, exceptionMessage);
        }
        return nullptr;
      }
      consume(TokenType::Greater, "'>' expected for sampler type");
    }
  }

  discardRestorePoint();

  AstType* type = _ast->createNode<AstType>();
  type->flags = flags;
  type->baseType = baseType;
  type->samplerType = sType;

  return type;
}

bool Parser::parseTypeModifier(uint32_t& flags) {
  if (match(TokenType::Const)) {
    flags |= TypeFlags::Const;
    return true;
  }
  
  if (match(TokenType::Static)) {
    flags |= TypeFlags::Static;
    return true;
  }

  if (match(TokenType::Uniform)) {
    return true;
  }

  if (match(TokenType::Inline)) {
    return true;
  }

  if (match(TokenType::In)) {
    flags |= TypeFlags::Input;
    return true;
  }

  if (match(TokenType::Out)) {
    flags |= TypeFlags::Output;
    return true;
  }

  if (match(TokenType::Inout)) {
    flags |= TypeFlags::Input;
    flags |= TypeFlags::Output;
    return true;
  }

  return false;
}

bool Parser::parseInterpolationModifier(uint32_t& flags) {
  if (match(TokenType::Linear)) {
    flags |= TypeFlags::Linear;
    return true;
  }

  if (match(TokenType::Centroid)) {
    flags |= TypeFlags::Centroid;
    return true;
  }

  if (match(TokenType::Nointerpolation)) {
    flags |= TypeFlags::Nointerpolation;
    return true;
  }

  if (match(TokenType::Noperspective)) {
    flags = flags | TypeFlags::Noperspective;
    return true;
  }

  if (match(TokenType::Sample)) {
    flags = flags | TypeFlags::Sample;
    return true;
  }

  return false;
}

AstExpression* Parser::parseLogicalOrExpression() {
  AstExpression* expr = parseLogicalAndExpression();
  while (match(TokenType::PipePipe)) {
    AstBinaryExpr* op = _ast->createNode<AstBinaryExpr>();
    op->op = Operator::OrOr;
    op->left = expr;
    op->right = parseLogicalAndExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseLogicalAndExpression() {
  AstExpression* expr = parseInclusiveOrExpression();
  while (match(TokenType::AmpersandAmpersand)) {
    AstBinaryExpr* op = _ast->createNode<AstBinaryExpr>();
    op->op = Operator::AndAnd;
    op->left = expr;
    op->right = parseInclusiveOrExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseInclusiveOrExpression() {
  AstExpression* expr = parseExclusiveOrExpression();
  while (match(TokenType::Pipe)) {
    AstBinaryExpr* op = _ast->createNode<AstBinaryExpr>();
    op->op = Operator::Or;
    op->left = expr;
    op->right = parseExclusiveOrExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseExclusiveOrExpression() {
  AstExpression* expr = parseAndExpression();
  while (match(TokenType::Caret)) {
    AstBinaryExpr* op = _ast->createNode<AstBinaryExpr>();
    op->op = Operator::Xor;
    op->left = expr;
    op->right = parseAndExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseAndExpression() {
  AstExpression* expr = parseEqualityExpression();
  while (match(TokenType::Ampersand)) {
    AstBinaryExpr* op = _ast->createNode<AstBinaryExpr>();
    op->op = Operator::And;
    op->left = expr;
    op->right = parseEqualityExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseEqualityExpression() {
  AstExpression* expr = parseRelationalExpression();
  while (check(TokenType::EqualEqual) || check(TokenType::BangEqual)) {
    Token tk = advance();
    AstBinaryExpr* op = _ast->createNode<AstBinaryExpr>();
    op->op = tk.type() == TokenType::EqualEqual ? Operator::EqualEqual : Operator::NotEqual;
    op->left = expr;
    op->right = parseRelationalExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseRelationalExpression() {
  AstExpression* expr = parseShiftExpression();
  while (check(TokenType::Greater) || check(TokenType::Less) ||
        check(TokenType::GreaterEqual) || check(TokenType::LessEqual)) {
    Token tk = advance();
    AstBinaryExpr* op = _ast->createNode<AstBinaryExpr>();
    op->op = tk.type() == TokenType::Greater ? Operator::Greater :
             tk.type() == TokenType::Less ? Operator::Less :
             tk.type() == TokenType::GreaterEqual ? Operator::GreaterEqual :
             Operator::LessEqual;
    op->left = expr;
    op->right = parseShiftExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseShiftExpression() {
  AstExpression* expr = parseAdditiveExpression();
  while (check(TokenType::GreaterGreater) || check(TokenType::LessLess)) {
    Token tk = advance();
    AstBinaryExpr* op = _ast->createNode<AstBinaryExpr>();
    op->op = tk.type() == TokenType::GreaterGreater ? Operator::RightShift :
             Operator::LeftShift;
    op->left = expr;
    op->right = parseAdditiveExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseAdditiveExpression() {
  AstExpression* expr = parseMultiplicativeExpression();
  while (check(TokenType::Plus) || check(TokenType::Minus)) {
    Token tk = advance();
    AstBinaryExpr* op = _ast->createNode<AstBinaryExpr>();
    op->op = tk.type() == TokenType::Plus ? Operator::Add : Operator::Subtract;
    op->left = expr;
    op->right = parseMultiplicativeExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseMultiplicativeExpression() {
  AstExpression* expr = parsePrefixExpression();
  while (check(TokenType::Star) || check(TokenType::Slash) || check(TokenType::Percent) ||
      check(TokenType::PlusPlus) || check(TokenType::MinusMinus)) {
    Token tk = advance();
    AstBinaryExpr* op = _ast->createNode<AstBinaryExpr>();
    op->op = tk.type() == TokenType::Star ? Operator::Multiply :
             tk.type() == TokenType::Slash ? Operator::Divide : Operator::Modulo;
    op->left = expr;
    op->right = parsePrefixExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parsePrefixExpression() {
  if (check(TokenType::Minus) || check(TokenType::Bang) || check(TokenType::Tilde) ||
      check(TokenType::PlusPlus) || check(TokenType::MinusMinus)) {
    Token tk = advance();
    AstPrefixExpr* op = _ast->createNode<AstPrefixExpr>();
    op->op = tk.type() == TokenType::Minus ? Operator::Subtract :
             tk.type() == TokenType::Bang ? Operator::Not :
             tk.type() == TokenType::Tilde ? Operator::BitNot :
             tk.type() == TokenType::PlusPlus ? Operator::PlusPlus :
             Operator::MinusMinus;
    op->expression = parsePrefixExpression();
    return op;
  }
  return parseSingularExpression();
}

AstExpression* Parser::parseSingularExpression() {
  AstExpression* expr = parsePrimaryExpression();
  expr = parsePostfixExpression(expr);

  if (match(TokenType::Question)) {
    AstTernaryExpr* op = _ast->createNode<AstTernaryExpr>();
    op->condition = expr;
    op->trueExpr = parseExpression();
    consume(TokenType::Colon, "Expected ':' after ternary expression");
    op->falseExpr = parseExpression();
    expr = op;
  }

  return expr;
}

AstExpression* Parser::parsePostfixExpression(AstExpression* expr) {
  // ++, --, [], .
  if (check(TokenType::PlusPlus) || check(TokenType::MinusMinus)) {
    Token tk = advance();
    AstIncrementExpr* op = _ast->createNode<AstIncrementExpr>();
    op->op = tk.type() == TokenType::PlusPlus ? Operator::PlusPlus : Operator::MinusMinus;
    op->variable = expr;
    return op;
  }

  if (match(TokenType::LeftBracket)) {
    AstExpression* index = parseExpression();
    consume(TokenType::RightBracket, "Expected ']' after expression");
    AstArrayExpr* array = _ast->createNode<AstArrayExpr>();
    array->array = expr;
    array->index = index;
    return array;
  }

  if (match(TokenType::Dot)) {
    AstMemberExpr* member = _ast->createNode<AstMemberExpr>();
    member->object = expr;
    member->member = parseSingularExpression();   
    return member;
  }

  return expr;
}

AstExpression* Parser::parsePrimaryExpression() {
  if (check(TokenType::Identifier)) {
    // An identifier is either a variable or a function call
    Token name = advance();
    if (check(TokenType::LeftParen)) {
      AstCallExpr* expr = _ast->createNode<AstCallExpr>();
      expr->name = name.lexeme();
      expr->arguments = parseArgumentList();
      return expr;
    }

    AstVariableExpr* expr = _ast->createNode<AstVariableExpr>();
    expr->name = name.lexeme();
    return expr;
  }

  if (check(TokenType::IntLiteral) || check(TokenType::FloatLiteral) || check(TokenType::True) ||
      check(TokenType::False)) {
    Token tk = advance();
    AstLiteralExpr* expr = _ast->createNode<AstLiteralExpr>();
    expr->type = tokenTypeToBaseType(tk.type());
    expr->value = tk.lexeme();
    return expr;
  }

  if (check(TokenType::LeftParen)) {
    return parseParenthesizedExpression();
  }

  Token tk = peekNext();
  // float(x) is a cast expression
  if (isType(tk)) {
    AstCastExpr* expr = _ast->createNode<AstCastExpr>();
    expr->type = parseType(false, "Invalid type");
    consume(TokenType::LeftParen, "Expected '(' after type");
    expr->expression = parseExpressionList();
    consume(TokenType::RightParen, "Expected ')' after expression");
    return expr;
  }

  return nullptr;
}

AstExpression* Parser::parseParenthesizedExpression() {
  consume(TokenType::LeftParen, "Expected '(' after expression");
  AstExpression* expr = parseLogicalOrExpression();
  consume(TokenType::RightParen, "Expected ')' after expression");
  return expr;
}

AstExpression* Parser::parseArgumentList() {
  consume(TokenType::LeftParen, "Expected '(' after function name");
  AstExpression* firstExpr = parseLogicalOrExpression();
  AstExpression* lastExpr = firstExpr;
  while (match(TokenType::Comma)) {
    AstExpression* expr = parseLogicalOrExpression();
    lastExpr->next = expr;
    lastExpr = expr;
  }
  consume(TokenType::RightParen, "Expected ')' after argument list");
  return firstExpr;
}

AstFunctionStmt* Parser::parseFunctionStmt(AstType* returnType, const std::string_view& name) {
  AstFunctionStmt* func = _ast->createNode<AstFunctionStmt>();
  func->returnType = returnType;
  func->name = name;
  func->parameters = parseParameterList();
  func->body = parseBlock();
  return func;
}

AstParameter* Parser::parseParameterList() {
  consume(TokenType::LeftParen, "Expected '(' after function name");
  if (match(TokenType::RightParen)) {
    // Empty parameter list
    return nullptr;
  }

  AstParameter* firstParam = parseParameter();
  AstParameter* lastParam = firstParam;
  while (match(TokenType::Comma)) {
    AstParameter* param = parseParameter();
    lastParam->next = param;
    lastParam = param;
  }

  consume(TokenType::RightParen, "Expected ')' after parameter list");
  return firstParam;
}

AstParameter* Parser::parseParameter() {
  AstParameter* param = _ast->createNode<AstParameter>();
  param->type = parseType(false, "Expected parameter type");
  param->name = consume(TokenType::Identifier, "Expected parameter name").lexeme();
  if (match(TokenType::Equal)) {
    param->initializer = parseExpression();
  }
  return param;
}

AstVariableStmt* Parser::parseVariableStmt(AstType* type, const std::string_view& name) {
  AstVariableStmt* var = _ast->createNode<AstVariableStmt>();
  var->type = type;
  var->name = name;
  if (match(TokenType::Equal)) {
    var->initializer = parseLogicalOrExpression();
  }
  consume(TokenType::Semicolon, "Expected ';' after variable declaration");
  return var;
}

// Parse a block of statements enclosed in braces
AstBlock* Parser::parseBlock() {
  AstBlock* block = _ast->createNode<AstBlock>();

  consume(TokenType::LeftBrace, "Expected '{' before block");

  // Empty block {}
  if (match(TokenType::RightBrace)) {
    return block;
  }

  block->statements = parseStatement();
  if (block->statements == nullptr) {
    return block;
  }

  AstStatement* lastStmt = block->statements;
  while (!check(TokenType::RightBrace) && !isAtEnd()) {
    AstStatement* stmt = parseStatement();
    if (stmt == nullptr) {
      throw ParseException(peekNext(), "Expected statement");
    }
    lastStmt->next = stmt;
    lastStmt = stmt;
  }

  consume(TokenType::RightBrace, "Expected '}' after block");
  return block;
}

AstStatement* Parser::parseStatement() {
  while (match(TokenType::Semicolon)) {
    // Ignore empty statements
  }

  AstAttribute* attributes = parseAttributes();

  if (match(TokenType::If)) {
    AstIfStmt* stmt = parseIfStmt();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::Switch)) {
    AstSwitchStmt* stmt = parseSwitchStmt();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::For)) {
    AstForStmt* stmt = parseForStmt();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::Do)) {
    AstDoWhileStmt* stmt = parseDoWhileStmt();
    stmt->attributes = attributes;
    consume(TokenType::Semicolon, "Expected ';' after return value");
    return stmt;
  }

  if (match(TokenType::While)) {
    AstWhileStmt* stmt = parseWhileStmt();
    stmt->attributes = attributes;
    return stmt;
  }

  if (check(TokenType::LeftBrace)) {
    AstStatement* stmt = parseBlock();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::Return)) {
    AstReturnStmt* stmt = _ast->createNode<AstReturnStmt>();
    stmt->attributes = attributes;

    if (check(TokenType::Semicolon)) {
      return stmt;
    }

    stmt->value = parseExpression();

    consume(TokenType::Semicolon, "Expected ';' after return value");

    return stmt;
  }

  if (match(TokenType::Break)) {
    AstBreakStmt* stmt = _ast->createNode<AstBreakStmt>();
    stmt->attributes = attributes;
    consume(TokenType::Semicolon, "Expected ';' after 'break'");
    return stmt;
  }

  if (match(TokenType::Continue)) {
    AstContinueStmt* stmt = _ast->createNode<AstContinueStmt>();
    stmt->attributes = attributes;
    consume(TokenType::Semicolon, "Expected ';' after 'continue'");
    return stmt;
  }

  if (match(TokenType::Discard)) {
    AstDiscardStmt* stmt = _ast->createNode<AstDiscardStmt>();
    stmt->attributes = attributes;
    consume(TokenType::Semicolon, "Expected ';' after 'discard'");
    return stmt;
  }

  AstStatement* stmt = nullptr;

  if (check(TokenType::Identifier)) {
    Token name = advance();
    if (check(TokenType::LeftParen)) {
      AstCallStmt* call = _ast->createNode<AstCallStmt>();
      call->name = name.lexeme();
      call->arguments = parseArgumentList();
      call->attributes = attributes;
      consume(TokenType::Semicolon, "Expected ';' after statment");
      return call;
    }

    pushBack(name);
  }

  AstType* type = parseType(false);
  if (type != nullptr) {
    const std::string_view name = consume(TokenType::Identifier, "Expected variable name").lexeme();
    stmt = parseVariableStmt(type, name);
    stmt->attributes = attributes;
    return stmt;
  }

  if (check(TokenType::Underscore) || check(TokenType::Identifier)) {
    const bool isUnderscore = match(TokenType::Underscore);

    AstAssignmentStmt* stmt = _ast->createNode<AstAssignmentStmt>();
    if (!isUnderscore) {
      stmt->variable = parsePrefixExpression();
    }

    if (!isUnderscore && stmt->variable == nullptr) {
      throw ParseException(peekNext(), "Expected variable name");
    }

    if (match(TokenType::Equal)) {
      stmt->op = Operator::Equal;
    } else if (match(TokenType::PlusEqual)) {
      stmt->op = Operator::AddEqual;
    } else if (match(TokenType::MinusEqual)) {
      stmt->op = Operator::SubtractEqual;
    } else if (match(TokenType::StarEqual)) {
      stmt->op = Operator::MultiplyEqual;
    } else if (match(TokenType::SlashEqual)) {
      stmt->op = Operator::DivideEqual;
    } else if (match(TokenType::PercentEqual)) {
      stmt->op = Operator::ModuloEqual;
    } else if (match(TokenType::LessLessEqual)) {
      stmt->op = Operator::LeftShiftEqual;
    } else if (match(TokenType::GreaterGreaterEqual)) {
      stmt->op = Operator::RightShiftEqual;
    } else if (match(TokenType::AmpersandEqual)) {
      stmt->op = Operator::AndEqual;
    } else if (match(TokenType::CaretEqual)) {
      stmt->op = Operator::XorEqual;
    } else if (match(TokenType::PipeEqual)) {
      stmt->op = Operator::OrEqual;
    } else {
      throw ParseException(peekNext(), "Expected assignment operator");
    }

    stmt->value = parseLogicalOrExpression();

    consume(TokenType::Semicolon, "Expected ';' after assignment");
    
    stmt->attributes = attributes;
    return stmt;
  }

  try {
    AstExpression* expr = parseExpression();
    if (expr != nullptr) {
      AstExpressionStmt* stmt = _ast->createNode<AstExpressionStmt>();
      stmt->expression = expr;
    }
  } catch (ParseException& e) {
  }

  if (stmt != nullptr) {
    consume(TokenType::Semicolon, "Expected ';' after expression");
    stmt->attributes = attributes;
    return stmt;
  }

  return nullptr;
}

AstIfStmt* Parser::parseIfStmt() {
  AstIfStmt* stmt = _ast->createNode<AstIfStmt>();

  consume(TokenType::LeftParen, "Expected '(' after 'if'");

  stmt->condition = parseExpression();

  consume(TokenType::RightParen, "Expected ')' after 'if' condition");

  stmt->body = parseStatement();

  if (match(TokenType::Else)) {
    stmt->elseBody = parseStatement();
  }

  return stmt;
}

AstSwitchStmt* Parser::parseSwitchStmt() {
  AstSwitchStmt* stmt = _ast->createNode<AstSwitchStmt>();
  consume(TokenType::LeftParen, "Expected '(' after 'switch'");
  stmt->condition = parseExpression();
  consume(TokenType::RightParen, "Expected ')' after 'switch' condition");
  consume(TokenType::LeftBrace, "Expected '{' after 'switch' condition");
  AstSwitchCase* lastCase = nullptr;
  while (!check(TokenType::RightBrace) && !isAtEnd()) {
    AstSwitchCase* caseStmt = _ast->createNode<AstSwitchCase>();
    if (match(TokenType::Case)) {      
      caseStmt->isDefault = false;
      caseStmt->condition = parseExpression();
    } else if (match(TokenType::Default)) {
      caseStmt->isDefault = true;
    } else {
      throw ParseException(peekNext(), "Expected 'case' or 'default' in switch statement");
    }

    consume(TokenType::Colon, "Expected ':' after 'case' or 'default'");
    caseStmt->body = parseStatement();

    if (stmt->cases == nullptr) {
      lastCase = caseStmt;
      stmt->cases = caseStmt;
    } else {
      lastCase->next = caseStmt;
    }
  }
  return stmt;
}

AstForStmt* Parser::parseForStmt() {
  AstForStmt* stmt = _ast->createNode<AstForStmt>();

  consume(TokenType::LeftParen, "Expected '(' after 'for'");
  if (!check(TokenType::Semicolon)) {
    stmt->initializer = parseStatement();
  }
  consume(TokenType::Semicolon, "Expected ';' after 'for' initializer");
  if (!check(TokenType::Semicolon)) {
    stmt->condition = parseExpression();
  }
  consume(TokenType::Semicolon, "Expected ';' after 'for' condition");
  if (!check(TokenType::RightParen)) {
    stmt->increment = parseExpression();
  }
  consume(TokenType::RightParen, "Expected ')' after 'for' increment");

  stmt->body = parseStatement();

  return stmt;
}

AstDoWhileStmt* Parser::parseDoWhileStmt() {
  AstDoWhileStmt* stmt = _ast->createNode<AstDoWhileStmt>();

  stmt->body = parseBlock();

  consume(TokenType::While, "Expected 'while' after 'do' block");
  consume(TokenType::LeftParen, "Expected '(' after 'while'");
  stmt->condition = parseExpression();
  consume(TokenType::RightParen, "Expected ')' after 'while' condition");

  return stmt;
}

AstWhileStmt* Parser::parseWhileStmt() {
  AstWhileStmt* stmt = _ast->createNode<AstWhileStmt>();
  consume(TokenType::LeftParen, "Expected '(' after 'while'");
  stmt->condition = parseExpression();
  consume(TokenType::RightParen, "Expected ')' after 'while' condition");
  stmt->body = parseStatement();
  return stmt;
}

} // namespace hlsl
