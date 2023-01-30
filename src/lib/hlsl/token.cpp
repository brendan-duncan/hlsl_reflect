#include <sstream>
#include "literal.h"
#include "token.h"

namespace hlsl {

std::string Token::toString() const {
  std::stringstream ss;
  ss << "Line:" << _line << " Lexeme:" << _lexeme << " Type:" << hlsl::tokenTypeToString(_type);
  return ss.str();
}

} // namespace hlsl
