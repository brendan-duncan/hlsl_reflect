#pragma once

#include <list>

#include "ast/ast.h"
#include "scanner.h"
#include "token.h"

namespace hlsl {

class Parser {
public:
  Parser(const std::string_view& source);

  Ast* parse();

private:
  bool isAtEnd();

  Token consume(TokenType type, const char* message = nullptr);

  Token advance();

  const Token& peekNext();

  bool match(TokenType type);

  bool check(TokenType type);

  AstStatement* parseTopLevel();

  AstStruct* parseStruct();

  Ast* _ast = nullptr;
  Scanner _scanner;
  std::list<Token> _pending;
};

} // namespace hlsl
