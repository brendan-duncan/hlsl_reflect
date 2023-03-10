#pragma once 

namespace ast {

enum class NodeType {
  Undefined,
  Type,
  Root,
  Field,
  Parameter,
  Attribute,
  SamplerState,
  StateAssignment,
  SwitchCase,
  Block,
  TemplateArg,

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
  TypedefStmt,
  EmptyStmt,

  // Expression Nodes
  PrefixExpr,
  IncrementExpr,
  ArrayExpr,
  MemberExpr,
  BinaryExpr,
  TernaryExpr,
  StringExpr,
  CallExpr,
  VariableExpr,
  LiteralExpr,
  CastExpr,
  AssignmentExpr,
  StructInitializerExpr,
  ArrayInitializerExpr,
};

} // namespace ast

