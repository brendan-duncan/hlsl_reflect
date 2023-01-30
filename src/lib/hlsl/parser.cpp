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
      statement = parseTopLevel();
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

AstStatement* Parser::parseTopLevel() {
  // semicolon

  while (match(TokenType::Semicolon) && !isAtEnd()) {}

  if (match(TokenType::Struct)) {
    return parseStruct();
  }

  return nullptr;
}

AstStruct* Parser::parseStruct() {
  Token name = consume(TokenType::Identifier, "struct name expected.");
  consume(TokenType::LeftBrace, "'{' expected for struct");
  consume(TokenType::RightBrace, "'}' expected for struct");
  consume(TokenType::Semicolon, "';' expected for struct");

  AstStruct* s = _ast->createNode<AstStruct>();
  s->name = name.lexeme();
  return s;
}

} // namespace hlsl
