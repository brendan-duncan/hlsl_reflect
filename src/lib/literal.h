#pragma once

#include <optional>
#include <string>
#include <variant>

namespace hlsl {

using Literal = std::variant<int, float, std::string>;
using OptionalLiteral = std::optional<Literal>;

std::string GetLiteralString(const Literal& literal);

auto MakeOptionalLiteral(int value) -> OptionalLiteral {
    return OptionalLiteral(std::in_place, value);
}

auto MakeOptionalLiteral(float value) -> OptionalLiteral {
    return OptionalLiteral(std::in_place, value);
}

auto MakeOptionalLiteral(const std::string& value) -> OptionalLiteral {
    return OptionalLiteral(std::in_place, value);
}

} // namespace hlsl
