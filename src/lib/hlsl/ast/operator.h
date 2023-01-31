#pragma once

#include "../token_type.h"

namespace hlsl {
  
enum class Operator {
  Undefined,
  OrOr,
  AndAnd,
  Or,
  Xor,
  And,
  EqualEqual,
  NotEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  LeftShift,
  RightShift,
  Add,
  Subtract,
  Multiply,
  Divide,
  Modulo,
  Not,
  AddEqual,
  SubtractEqual,
  MultiplyEqual,
  DivideEqual,
  ModuloEqual,
  BitNot,
  Dereference,
  AddressOf
};

} // namespace hlsl
