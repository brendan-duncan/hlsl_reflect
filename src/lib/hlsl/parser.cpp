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
      delete _ast;
      _ast = nullptr;
      return nullptr;
    }
    if (statement == nullptr) {
      break;
    }
    if (root->statement == nullptr) {
      root->statement = statement;
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
    AstStruct* node = parseStruct();
    if (node != nullptr) {
      node->attributes = attributes;
    }
    return node;
  }

  if (match(TokenType::Cbuffer) || match(TokenType::Tbuffer)) {
    AstBuffer* node = parseBuffer();
    if (node != nullptr) {
      node->attributes = attributes;
    }
    return node;
  }

  // Global Declaration
  AstType* type = parseType(true/*allowVoid*/);
  if (type != nullptr) {
    // Either a variable or function declaration.
    Token identifier = consume(TokenType::Identifier, "identifier expected");

    // function declaration
    if (check(TokenType::LeftParen)) {
      AstFunction* func = parseFunction(type, identifier.lexeme());
      if (func != nullptr) {
        func->attributes = attributes;
      }
      return func;
    }

    // variable declaration
    AstVariable* var = parseVariable(type, identifier.lexeme());
    if (var != nullptr) {
      var->attributes = attributes;
    }
    return var;
  }

  return nullptr;
}

AstStruct* Parser::parseStruct() {
  Token name = consume(TokenType::Identifier, "struct name expected.");
  consume(TokenType::LeftBrace, "'{' expected for struct");

  AstStructField* lastField = nullptr;
  while (!check(TokenType::RightBrace) && !isAtEnd()) {
    AstStructField* field = parseStructField();

    if (lastField != nullptr) {
      lastField->next = field;
    }
    lastField = field;
  }

  consume(TokenType::RightBrace, "'}' expected for struct");
  consume(TokenType::Semicolon, "';' expected for struct");

  AstStruct* s = _ast->createNode<AstStruct>();
  s->name = name.lexeme();
  return s;
}

AstStructField* Parser::parseStructField() {
  AstStructField* field = _ast->createNode<AstStructField>();

  Token tk = peekNext();
  if (isInterpolationModifier(tk.type())) {
    field->interpolation = tokenTypeToInterpolationModifier(tk.type());
    advance();
  }

  field->type = advance().type();
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
      AstArgument* firstArg = nullptr;
      AstArgument* lastArg = nullptr;
      while (!isAtEnd() && !match(TokenType::RightParen)) {
        AstArgument* arg = _ast->createNode<AstArgument>();
        arg->expression = parseExpression();
        if (firstArg == nullptr) {
          firstArg = arg;
        }
        if (lastArg != nullptr) {
          lastArg->next = arg;
        }
        lastArg = arg;
        if (arg->expression == nullptr) {
          throw ParseException(peekNext(), "expression expected for attribute");
          break;
        }

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
  return parseShortCircuitOrExpression();
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

AstDeclaration* Parser::parseDeclaration() {
  AstType* type = parseType(false);

  AstDeclaration* lastDecl = nullptr;
  AstDeclaration* firstDecl = nullptr;

  do {
    AstDeclaration* decl = _ast->createNode<AstDeclaration>();
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

AstBuffer* Parser::parseBuffer() {
  AstBuffer* buffer = _ast->createNode<AstBuffer>();
  buffer->name = consume(TokenType::Identifier, "buffer name expected").lexeme();
  if (match(TokenType::Colon)) {
    consume(TokenType::Register, "register expected");
    consume(TokenType::LeftParen, "'(' expected");
    buffer->registerName = consume(TokenType::Identifier, "register name expected").lexeme();
    consume(TokenType::RightParen, "')' expected");
  }
  consume(TokenType::LeftBrace, "'{' expected for buffer");
  
  AstDeclaration* lastField = nullptr;
  while (!match(TokenType::RightBrace) && !isAtEnd()) {
    AstDeclaration* field = parseDeclaration();
    if (field == nullptr) {
      throw ParseException(peekNext(), "declaration expected for buffer");
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

AstType* Parser::parseType(bool allowVoid) {
  AstType* type = _ast->createNode<AstType>();

  while (parseTypeModifier(type->flags) || parseInterpolationModifier(type->flags)) {}

  Token token = advance();

  if (token.type() == TokenType::Identifier) {
    type->baseType = BaseType::UserDefined;
    type->name = token.lexeme();
    return type;
  }

  type->baseType = tokenTypeToBaseType(token.type());

  if (type->baseType == BaseType::Undefined) {
    throw ParseException(peekNext(), "unknown type");
  }

  if (type->baseType == BaseType::Void) {
    if (allowVoid) {
      return type;
    } else {
      throw ParseException(peekNext(), "void type not allowed");
    }
  }
 
  if (isSamplerBaseType(type->baseType)) {
    if (match(TokenType::Less)) {
      Token samplerType = advance();
      type->samplerType = tokenTypeToSamplerType(samplerType.type());
      if (type->samplerType == SamplerType::Undefined) {
        throw ParseException(samplerType, "unknown sampler type");
      }
      consume(TokenType::Greater, "'>' expected for sampler type");
    }
  }

  return type;
}

bool Parser::parseTypeModifier(TypeFlags& flags) {
  if (match(TokenType::Const)) {
    flags = flags | TypeFlags::Const;
    return true;
  }
  
  if (match(TokenType::Static)) {
    flags = flags | TypeFlags::Static;
    return true;
  }

  if (match(TokenType::Uniform)) {
    return true;
  }

  if (match(TokenType::Inline)) {
    return true;
  }

  return false;
}

bool Parser::parseInterpolationModifier(TypeFlags& flags) {
  if (match(TokenType::Linear)) {
    flags = flags | TypeFlags::Linear;
    return true;
  }

  if (match(TokenType::Centroid)) {
    flags = flags | TypeFlags::Centroid;
    return true;
  }

  if (match(TokenType::Nointerpolation)) {
    flags = flags | TypeFlags::Nointerpolation;
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

AstExpression* Parser::parseShortCircuitOrExpression() {
  AstExpression* expr = parseShortCircuitAndExpression();
  while (match(TokenType::PipePipe)) {
    AstBinaryOperator* op = _ast->createNode<AstBinaryOperator>();
    op->op = Operator::OrOr;
    op->left = expr;
    op->right = parseShortCircuitAndExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseShortCircuitAndExpression() {
  AstExpression* expr = parseInclusiveOrExpression();
  while (match(TokenType::AmpersandAmpersand)) {
    AstBinaryOperator* op = _ast->createNode<AstBinaryOperator>();
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
    AstBinaryOperator* op = _ast->createNode<AstBinaryOperator>();
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
    AstBinaryOperator* op = _ast->createNode<AstBinaryOperator>();
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
    AstBinaryOperator* op = _ast->createNode<AstBinaryOperator>();
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
    AstBinaryOperator* op = _ast->createNode<AstBinaryOperator>();
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
    AstBinaryOperator* op = _ast->createNode<AstBinaryOperator>();
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
    AstBinaryOperator* op = _ast->createNode<AstBinaryOperator>();
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
    AstBinaryOperator* op = _ast->createNode<AstBinaryOperator>();
    op->op = tk.type() == TokenType::Plus ? Operator::Add : Operator::Subtract;
    op->left = expr;
    op->right = parseMultiplicativeExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseMultiplicativeExpression() {
  AstExpression* expr = parseUnaryExpression();
  while (check(TokenType::Star) || check(TokenType::Slash) || check(TokenType::Percent)) {
    Token tk = advance();
    AstBinaryOperator* op = _ast->createNode<AstBinaryOperator>();
    op->op = tk.type() == TokenType::Star ? Operator::Multiply :
             tk.type() == TokenType::Slash ? Operator::Divide : Operator::Modulo;
    op->left = expr;
    op->right = parseUnaryExpression();
    expr = op;
  }
  return expr;
}

AstExpression* Parser::parseUnaryExpression() {
  if (check(TokenType::Minus) || check(TokenType::Bang) || check(TokenType::Tilde) ||
      check(TokenType::Star) || check(TokenType::Ampersand)) {
    Token tk = advance();
    AstUnaryOperator* op = _ast->createNode<AstUnaryOperator>();
    op->op = tk.type() == TokenType::Minus ? Operator::Subtract :
             tk.type() == TokenType::Bang ? Operator::Not :
             tk.type() == TokenType::Tilde ? Operator::BitNot :
             tk.type() == TokenType::Star ? Operator::Dereference :
             Operator::AddressOf;
    op->expression = parseUnaryExpression();
    return op;
  }
  return parseSingularExpression();
}

AstExpression* Parser::parseSingularExpression() {
  AstExpression* expr = parsePrimaryExpression();
  AstExpression* p = parsePostfixExpression();
  if (p != nullptr) {
    expr->postfix = p;
  }
  return expr;
}

AstExpression* Parser::parsePostfixExpression() {
  if (match(TokenType::LeftBracket)) {
    AstExpression* expr = parseShortCircuitOrExpression();
    consume(TokenType::RightBracket, "Expected ']' after expression");
    AstExpression* p = parsePostfixExpression();
    if (p != nullptr) {
      expr->postfix = p;
    }
    return expr;
  }

  if (match(TokenType::Dot)) {
    Token name = consume(TokenType::Identifier, "Expected identifier after '.'");
    AstExpression* p = parsePostfixExpression();
    AstStringExpr* expr = _ast->createNode<AstStringExpr>();
    if (p != nullptr) {
      expr->postfix = p;
    }
    return expr;
  }

  return nullptr;
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

  if (check(TokenType::IntLiteral) || check(TokenType::FloatLiteral)) {
    Token tk = advance();
    AstLiteralExpr* expr = _ast->createNode<AstLiteralExpr>();
    expr->type = tokenTypeToBaseType(tk.type());
    expr->value = tk.lexeme();
    return expr;
  }

  if (check(TokenType::LeftParen)) {
    return parseParenthesizedExpression();
  }

  return nullptr;
}

AstExpression* Parser::parseParenthesizedExpression() {
  consume(TokenType::LeftParen, "Expected '(' after expression");
  AstExpression* expr = parseShortCircuitOrExpression();
  consume(TokenType::RightParen, "Expected ')' after expression");
  return expr;
}

AstExpression* Parser::parseArgumentList() {
  consume(TokenType::LeftParen, "Expected '(' after function name");
  AstExpression* firstExpr = parseShortCircuitOrExpression();
  AstExpression* lastExpr = firstExpr;
  while (match(TokenType::Comma)) {
    AstExpression* expr = parseShortCircuitOrExpression();
    lastExpr->next = expr;
    lastExpr = expr;
  }
  consume(TokenType::RightParen, "Expected ')' after argument list");
  return firstExpr;
}

AstFunction* Parser::parseFunction(AstType* returnType, const std::string_view& name) {
  AstFunction* func = _ast->createNode<AstFunction>();
  func->returnType = returnType;
  func->name = name;
  func->parameters = parseParameterList();
  consume(TokenType::LeftBrace, "Expected '{' before function body");
  func->body = parseBlock();
  return func;
}

AstParameter* Parser::parseParameterList() {
  consume(TokenType::LeftParen, "Expected '(' after function name");
  if (check(TokenType::RightParen)) {
    advance();
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
  param->type = parseType(false/*allowVoid*/);
  param->name = consume(TokenType::Identifier, "Expected parameter name").lexeme();
  return param;
}

AstVariable* Parser::parseVariable(AstType* type, const std::string_view& name) {
  AstVariable* var = _ast->createNode<AstVariable>();
  var->type = type;
  var->name = name;
  if (match(TokenType::Equal)) {
    var->initializer = parseShortCircuitOrExpression();
  }
  consume(TokenType::Semicolon, "Expected ';' after variable declaration");
  return var;
}

// Parse a block of statements enclosed in braces
AstStatement* Parser::parseBlock() {
  consume(TokenType::LeftBrace, "Expected '{' before block");
  AstStatement* firstStmt = parseStatement();
  AstStatement* lastStmt = firstStmt;
  while (!check(TokenType::RightBrace) && !isAtEnd()) {
    AstStatement* stmt = parseStatement();
    lastStmt->next = stmt;
    lastStmt = stmt;
  }
  consume(TokenType::RightBrace, "Expected '}' after block");
  return firstStmt;
}

AstStatement* Parser::parseStatement() {
  while (match(TokenType::Semicolon)) {
    // Ignore empty statements
  }

  AstAttribute* attributes = parseAttributes();

  if (match(TokenType::If)) {
    AstIf* stmt = parseIf();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::Switch)) {
    AstSwitch* stmt = parseSwitch();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::For)) {
    AstFor* stmt = parseFor();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::Do)) {
    AstDoWhile* stmt = parseDoWhile();
    stmt->attributes = attributes;
    consume(TokenType::Semicolon, "Expected ';' after return value");
    return stmt;
  }

  if (match(TokenType::While)) {
    AstWhile* stmt = parseWhile();
    stmt->attributes = attributes;
    return stmt;
  }

  if (check(TokenType::LeftBrace)) {
    AstStatement* stmt = parseBlock();
    stmt->attributes = attributes;
    return stmt;
  }

  if (match(TokenType::Return)) {
    AstReturn* stmt = _ast->createNode<AstReturn>();
    stmt->attributes = attributes;
    if (!check(TokenType::Semicolon)) {
      stmt->value = parseExpression();
      consume(TokenType::Semicolon, "Expected ';' after return value");
    }
    return stmt;
  }

  if (match(TokenType::Break)) {
    AstBreak* stmt = _ast->createNode<AstBreak>();
    stmt->attributes = attributes;
    consume(TokenType::Semicolon, "Expected ';' after 'break'");
    return stmt;
  }

  if (match(TokenType::Continue)) {
    AstContinue* stmt = _ast->createNode<AstContinue>();
    stmt->attributes = attributes;
    consume(TokenType::Semicolon, "Expected ';' after 'continue'");
    return stmt;
  }

  if (match(TokenType::Discard)) {
    AstDiscard* stmt = _ast->createNode<AstDiscard>();
    stmt->attributes = attributes;
    consume(TokenType::Semicolon, "Expected ';' after 'discard'");
    return stmt;
  }

  AstStatement* stmt = nullptr;
  try {
    stmt = parseDeclaration();
  } catch (ParseException& e) {
  }

  if (stmt != nullptr) {
    consume(TokenType::Semicolon, "Expected ';' after statement");
    stmt->attributes = attributes;
    return stmt;
  }

  try {
    AstExpression* expr = parseExpression();
    if (expr != nullptr) {
      AstExpressionStatement* stmt = _ast->createNode<AstExpressionStatement>();
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

AstIf* Parser::parseIf() {
  AstIf* stmt = _ast->createNode<AstIf>();

  consume(TokenType::LeftParen, "Expected '(' after 'if'");

  stmt->condition = parseExpression();

  consume(TokenType::RightParen, "Expected ')' after 'if' condition");

  stmt->body = parseStatement();

  if (check(TokenType::Else)) {
    stmt->elseBody = parseStatement();
  }

  return stmt;
}

AstSwitch* Parser::parseSwitch() {
  AstSwitch* stmt = _ast->createNode<AstSwitch>();
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

AstFor* Parser::parseFor() {
  AstFor* stmt = _ast->createNode<AstFor>();

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

AstDoWhile* Parser::parseDoWhile() {
  AstDoWhile* stmt = _ast->createNode<AstDoWhile>();

  stmt->body = parseBlock();

  consume(TokenType::While, "Expected 'while' after 'do' block");
  consume(TokenType::LeftParen, "Expected '(' after 'while'");
  stmt->condition = parseExpression();
  consume(TokenType::RightParen, "Expected ')' after 'while' condition");

  return stmt;
}

AstWhile* Parser::parseWhile() {
  AstWhile* stmt = _ast->createNode<AstWhile>();
  consume(TokenType::LeftParen, "Expected '(' after 'while'");
  stmt->condition = parseExpression();
  consume(TokenType::RightParen, "Expected ')' after 'while' condition");
  stmt->body = parseStatement();
  return stmt;
}

} // namespace hlsl
