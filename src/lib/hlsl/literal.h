#pragma once

#include <optional>
#include <string>
#include <variant>

#include "token_type.h"

namespace hlsl {

/// @brief Test the lexeme against one of the "literal" token type rules, either an int number,
/// float, or identifier.
/// @param lexeme The string to test.
/// @return TokenType::IntLiteral, TokenType::FloatLiteral, TokenType::Identifier,
/// or TokenType::Undefined if it wasn't matched to a literal.
TokenType matchLiteral(const std::string_view& lexeme);

using Literal = std::variant<int, float, std::string>;
using OptionalLiteral = std::optional<Literal>;

std::string GetLiteralString(const Literal& literal);

inline auto MakeOptionalLiteral(int value) -> OptionalLiteral {
    return OptionalLiteral(std::in_place, value);
}

inline auto MakeOptionalLiteral(float value) -> OptionalLiteral {
    return OptionalLiteral(std::in_place, value);
}

inline auto MakeOptionalLiteral(const std::string& value) -> OptionalLiteral {
    return OptionalLiteral(std::in_place, value);
}

} // namespace hlsl
