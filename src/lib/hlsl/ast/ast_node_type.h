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
  SwitchCase,

  // Statements
  Variable,
  Function,
  If,
  Switch,
  For,
  DoWhile,
  While,
  Discard,
  Break,
  Continue,
  Return,
  ExpressionStatement,

  // Expressions
  UnaryOperator,
  BinaryOperator,
  StringExpr,
  CallExpr,
  VariableExpr,
  LiteralExpr,
};

} // namespace hlsl

