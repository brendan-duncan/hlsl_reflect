#pragma once

#include <stdint.h>

namespace hlsl {

enum class TypeFlags : uint32_t {
  None = 0,

  Const = 0x01,
  Static = 0x02,
  
  Input = 0x100,
  Output = 0x200,
  
  Linear = 0x1000,
  Centroid = 0x2000,
  Nointerpolation = 0x4000,
  Noperspective = 0x8000,
  Sample = 0x10000,

  NoPromote = 0x20000
};

inline TypeFlags operator|(TypeFlags lhs, TypeFlags rhs) {
  return static_cast<TypeFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline TypeFlags operator&(TypeFlags lhs, TypeFlags rhs) {
  return static_cast<TypeFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

} // namespace hlsl
