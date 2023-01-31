#pragma once

#include <string_view>

namespace hlsl {

bool stringEqualCaseInsensitive(const std::string_view& a, const std::string_view& b);

int toInt(const std::string_view& str);

float toFloat(const std::string_view& str);

} // namespace hlsl
