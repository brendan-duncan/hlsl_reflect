#include "parser.h"

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

  while (isAtEnd()) {
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
  return nullptr;
}

} // namespace hlsl
