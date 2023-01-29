#pragma once

#include <string_view>
#include "literal.h"
#include "token_type.h"

namespace hlsl {

class Token {
public:
  Token(TokenType type, std::string lexeme, OptionalLiteral literal, int line)
    : _type(type)
    , _lexeme(lexeme)
    , _literal(literal)
    , _line(line) { }

  Token(TokenType type, const char* lexeme, OptionalLiteral literal, int line)
    : _type(type)
    , _lexeme(lexeme)
    , _literal(literal)
    , _line(line) { }

  Token(TokenType type, const char* lexeme, int line)
    : _type(type)
    , _lexeme(lexeme)
    , _line(line) { }

  std::string toString() const;

  TokenType type() const { return _type; }

  const std::string& lexeme() const { return _lexeme; }

  int line() const { return _line; }

private:
  const TokenType _type;
  const std::string _lexeme;
  OptionalLiteral _literal = std::nullopt;
  const int _line = -1;
};

} // namespace hlsl
