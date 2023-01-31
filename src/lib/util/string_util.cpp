#include "string_util.h"

#include <charconv>
#include <ctype.h>

namespace hlsl {

bool stringEqualCaseInsensitive(const std::string_view& a, const std::string_view& b) {
  const size_t aSize = a.size();
  if (aSize != b.size()) {
    return false;
  }
  if (a == b) {
    return true;
  }

  for (size_t i = 0; i < aSize; ++i) {
    if (tolower(a[i]) != tolower(b[i])) {
      return false;
    }
  }
  return true;
}

int toInt(const std::string_view& str) {
  int value = 0;
  std::from_chars(str.data(), str.data() + str.size(), value);
  return value;
}

float toFloat(const std::string_view& str) {
  float value = 0;
  std::from_chars(str.data(), str.data() + str.size(), value);
  return value;
}

} // namespace hlsl