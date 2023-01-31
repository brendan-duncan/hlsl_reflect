#pragma once

#include "../token.h"

namespace hlsl {

enum class AddressSpace {
  Undefined,
  Constant,
  Device,
  Thread,
  Shared,
};

inline AddressSpace tokenToAddressSpace(Token t) {
  if (t.type() == TokenType::Identifier) {
    if (t.lexeme() == "constant") {
      return AddressSpace::Constant;
    } else if (t.lexeme() == "device") {
      return AddressSpace::Device;
    } else if (t.lexeme() == "thread") {
      return AddressSpace::Thread;
    } else if (t.lexeme() == "shared") {
      return AddressSpace::Shared;
    }
  } else if (t.type() == TokenType::Shared) {
    return AddressSpace::Shared;
  }
  return AddressSpace::Undefined;
}

} // namespace hlsl
