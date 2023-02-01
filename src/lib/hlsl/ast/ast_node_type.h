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
  Block,

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
  Assignment,
  ExpressionStatement,

  // Expressions
  UnaryOperator,
  BinaryOperator,
  TernaryOperator,
  StringExpr,
  CallExpr,
  VariableExpr,
  LiteralExpr,  
};

} // namespace hlsl

