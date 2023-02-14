#include <sstream>
#include "token.h"

namespace reader {
namespace hlsl {

std::string Token::toString() const {
  std::stringstream ss;
  ss << "Lexeme:" << _lexeme << " Type:" << hlsl::tokenTypeToString(_type);
  return ss.str();
}

} // namespace hlsl
} // namespace reader
