#pragma once 

namespace hlsl {

enum class AstNodeType {
  Undefined,
  Type,
  Root,
  Expression,
  Struct,
  StructField,
  Argument,
  Attribute,
  Buffer,
  Declaration,
  SamplerState,
  StateAssignment,

  UnaryOperator,
  BinaryOperator,
  StringExpr,
  CallExpr,
  VariableExpr
};

} // namespace hlsl

