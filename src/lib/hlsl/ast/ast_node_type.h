#pragma once 

namespace hlsl {

enum class AstNodeType {
  Undefined,
  Type,
  Root,
  StructField,
  Parameter,
  Attribute,
  BufferField,
  SamplerState,
  StateAssignment,
  SwitchCase,
  Block,

  // Statement Nodes
  StructStmt,
  BufferStmt,
  VariableStmt,
  FunctionStmt,
  IfStmt,
  SwitchStmt,
  ForStmt,
  DoWhileStmt,
  WhileStmt,
  DiscardStmt,
  BreakStmt,
  ContinueStmt,
  ReturnStmt,
  AssignmentStmt,
  ExpressionStmt,
  CallStmt,

  // Expression Nodes
  UnaryExpr,
  BinaryExpr,
  TernaryExpr,
  StringExpr,
  CallExpr,
  VariableExpr,
  LiteralExpr,
  CastExpr,
};

} // namespace hlsl

