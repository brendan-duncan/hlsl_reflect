#pragma once

#include <optional>
#include <string>
#include <variant>

#include "../token_type.h"

namespace hlsl {

/// Test the lexeme against one of the "literal" token type rules, either an int number,
/// float, or identifier.
/// @param lexeme The string to test.
/// @return TokenType::IntLiteral, TokenType::FloatLiteral, TokenType::Identifier,
/// or TokenType::Undefined if it wasn't matched to a literal.
TokenType matchLiteral(const std::string_view& lexeme);

} // namespace hlsl
