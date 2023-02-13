#pragma once

#include <stdint.h>

namespace ast {

namespace TypeFlags {
  static const uint32_t None = 0;

  static const uint32_t Const = 0x01;
  static const uint32_t Static = 0x02;
  
  static const uint32_t Input = 0x100;
  static const uint32_t Output = 0x200;
  
  static const uint32_t Linear = 0x1000;
  static const uint32_t Centroid = 0x2000;
  static const uint32_t Nointerpolation = 0x4000;
  static const uint32_t Noperspective = 0x8000;
  static const uint32_t Sample = 0x10000;

  static const uint32_t NoPromote = 0x20000;

  static const uint32_t Unorm = 0x40000;
  static const uint32_t Snorm = 0x80000;
}

} // namespace ast
