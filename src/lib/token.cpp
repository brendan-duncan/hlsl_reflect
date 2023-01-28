#include <format>
#include "token.h"

namespace hlsl {

std::string Token::toString() const {
    std::string result = std::format("Line: {}, Lexeme: {}, Type: {}",
        _line, _lexeme, _type);
    if (_literal.has_value())
        result += std::format(", Literal: {}", GetLiteralString(_literal.value()));
}

} // namespace hlsl
