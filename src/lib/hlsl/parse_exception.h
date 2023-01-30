#pragma once

#include <exception>
#include <string_view>

#include "token.h"

namespace hlsl {

class ParseException : public std::exception {
public:
  Token token;
  std::string_view message;
  ParseException(const Token& token, const std::string_view& message)
    : token(token)
    , message(message) {}
};

} // namespace hlsl
