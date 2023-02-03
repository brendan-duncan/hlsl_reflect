#include "operator.h"

namespace hlsl {

const std::string_view& operatorToString(Operator op) {
  static const std::string_view strings[] = {
    "Undefined",
    "=",
    "-=",
    "*=",
    "/=",
    "%=",
    "<<=",
    ">>=",
    "&=",
    "|=",
    "^=",
    "++",
    "--",
    "||",
    "&&",
    "|",
    "^",
    "&",
    "==",
    "!=",
    "<",
    "<=",
    ">",
    ">=",
    "<<",
    ">>",
    "+",
    "-",
    "*",
    "/",
    "%",
    "!",
    "+=",
    "~",
    "*",
    "&"
  };
  return strings[(int)op];
}

} // namespace hlsl
