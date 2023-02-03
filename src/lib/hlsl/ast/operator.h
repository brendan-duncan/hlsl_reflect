#pragma once

#include "../token_type.h"

namespace hlsl {
  
enum class Operator {
  Undefined,

  Equal,
  SubtractEqual,
  MultiplyEqual,
  DivideEqual,
  ModuloEqual,
  LeftShiftEqual,
  RightShiftEqual,
  AndEqual,
  OrEqual,
  XorEqual,

  PlusPlus,
  MinusMinus,

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
  BitNot,
};

const std::string_view& operatorToString(Operator op);

} // namespace hlsl
