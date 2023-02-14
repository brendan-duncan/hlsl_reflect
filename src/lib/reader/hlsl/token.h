#pragma once

#include <string_view>
#include "token_type.h"

namespace reader {
namespace hlsl {

/// A token is a single lexeme with a type.
class Token {
public:
  Token()
      : _type(TokenType::Undefined) { }

  Token(TokenType type, std::string_view lexeme)
      : _type(type)
      , _lexeme(lexeme) { }

  Token(const Token& other)
      : _type(other._type)
      , _lexeme(other._lexeme) { }

  Token(const Token&& other)
      : _type(other._type)
      , _lexeme(std::move(other._lexeme)) { }

  Token& operator=(const Token& other) {
    _type = other._type;
    _lexeme = other._lexeme;
    return *this;
  }

  operator std::string() const { return toString(); }

  std::string toString() const;

  TokenType type() const { return _type; }

  const std::string_view& lexeme() const { return _lexeme; }

private:
  TokenType _type;
  std::string_view _lexeme;
};

} // namespace hlsl
} // namespace reader
