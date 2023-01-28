#include "literal.h"

namespace hlsl {

std::string GetLiteralString(const Literal& value) {
    // Literal = std::variant<std::string, double>;
  switch (value.index()) {
    case 0: // int
      return std::to_string(std::get<0>(value));
    case 1: {  // double
      std::string result = std::to_string(std::get<1>(value));
      auto pos = result.find(".000000");
      if (pos != std::string::npos)
        result.erase(pos, std::string::npos);
      else
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
      return result;
    }
    case 2:  // string
      return std::get<2>(value);
    default:
      static_assert(
          std::variant_size_v<Literal> == 3,
          "Looks like you forgot to update the cases in getLiteralString()!");
      return "";
  }
}

} // namespace hlsl
