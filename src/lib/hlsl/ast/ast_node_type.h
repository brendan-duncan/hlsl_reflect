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
  Parameter,
  Attribute,
  Buffer,
  Declaration,
  SamplerState,
  StateAssignment,
  Variable,
  Function,

  UnaryOperator,
  BinaryOperator,
  StringExpr,
  CallExpr,
  VariableExpr
};

} // namespace hlsl

