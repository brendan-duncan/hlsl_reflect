#pragma once

#include "../token_type.h"

namespace hlsl {

enum class SamplerType {
  Undefined,
  Float,
  Half
};

inline SamplerType tokenTypeToSamplerType(TokenType t) {
  if (t == TokenType::Float) {
    return SamplerType::Float;
  } else if (t == TokenType::Half) {
    return SamplerType::Half;
  }
  return SamplerType::Undefined;
}

} // namespace hlsl
