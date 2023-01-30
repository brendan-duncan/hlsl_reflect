#pragma once

#include <string_view>
#include "literal.h"
#include "token_type.h"

namespace hlsl {

class Token {
public:
  Token(TokenType type, std::string_view lexeme, int line)
      : _type(type)
      , _lexeme(lexeme)
      , _line(line) { }

  Token(const Token& other)
      : _type(other._type)
      , _lexeme(other._lexeme)
      , _filename(other._filename)
      , _line(other._line) { }

  Token(const Token&& other)
      : _type(other._type)
      , _lexeme(std::move(other._lexeme))
      , _filename(std::move(other._filename))
      , _line(other._line) { }

  std::string toString() const;

  TokenType type() const { return _type; }

  const std::string_view& lexeme() const { return _lexeme; }

  const std::string_view& filename() const { return _filename; }

  int line() const { return _line; }

private:
  const TokenType _type;
  const std::string_view _lexeme;
  const std::string_view _filename;
  const int _line = -1;
};

} // namespace hlsl
