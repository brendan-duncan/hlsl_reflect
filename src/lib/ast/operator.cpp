#include "operator.h"

namespace ast {

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

} // namespace ast
