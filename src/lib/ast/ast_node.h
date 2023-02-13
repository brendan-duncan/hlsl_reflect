#pragma once

#include <string_view>

#include "address_space.h"
#include "ast_node_type.h"
#include "base_type.h"
#include "buffer_type.h"
#include "interpolation_modifier.h"
#include "operator.h"
#include "sampler_type.h"
#include "type_flags.h"

namespace ast {

struct AssignmentStmt;
struct Attribute;
struct BinaryExpr;
struct Block;
struct BreakStmt;
struct BufferStmt;
struct CallExpr;
struct ContinueStmt;
struct DiscardStmt;
struct DoWhileStmt;
struct Expression;
struct ExpressionStmt;
struct ForStmt;
struct FunctionStmt;
struct IfStmt;
struct LiteralExpr;
struct CastExpr;
struct AssignmentExpr;
struct Node;
struct Parameter;
struct ReturnStmt;
struct Root;
struct SamplerState;
struct StateAssignment;
struct Statement;
struct StringExpr;
struct StructStmt;
struct Field;
struct SwitchStmt;
struct SwitchCase;
struct TernaryExpr;
struct Type;
struct TypedefStmt;
struct UnaryExpr;
struct VariableStmt;
struct VariableExpr;
struct WhileStmt;
struct CallStmt;

/// Call the callback for each node in the linked list
/// @tparam T Node with a next pointer, such as Expression
/// @param node The start of the linked list
/// @param callback The callback to call for each node
template<typename T>
void foreach(T* node, void (*callback)(T* node)) {
  while (node) {
    callback(node);
    node = node->next;
  }
}

/// Base class for all AST nodes
struct Node {
  NodeType nodeType = NodeType::Undefined;
};

/// A template argument, either an Type or an Expression.
struct TemplateArg : Node {
  static const NodeType astType = NodeType::TemplateArg;
  Node* value = nullptr;
  TemplateArg* next = nullptr;
};

/// Type declaration for a variable or function
struct Type : Node {
  static const NodeType astType = NodeType::Type;
  BaseType baseType = BaseType::Undefined;
  TemplateArg* templateArg = nullptr;
  std::string_view name; // The name of the type if it's a user defined type
  bool array = false;
  Expression* arraySize = nullptr;
  uint32_t flags = TypeFlags::None;
  AddressSpace addressSpace = AddressSpace::Undefined;

  inline bool isConst() const { return flags & TypeFlags::Const; }
  inline bool isStatic() const { return flags & TypeFlags::Static; }
};

/// Base class for all expressions
struct Expression : Node {
  Expression* next = nullptr;
};

/// Attributes augment a statement or expression with additional information.
struct Attribute : Node {
  static const NodeType astType = NodeType::Attribute;
  std::string_view name;
  Expression* argument = nullptr;
  Attribute* next = nullptr;
};

/// Base class for all statements
struct Statement : Node {
  Attribute* attributes = nullptr;
  Statement* next = nullptr;
};

/// Indicates an empty statement, such as a semicolon.
struct EmptyStatement : Statement {
  static const NodeType astType = NodeType::EmptyStmt;
  // There is only one instance of this class, so we can use a static instance.
  static EmptyStatement instance;
};

/// A field member of a struct or buffer.
struct Field : Node {
  static const NodeType astType = NodeType::Field;
  InterpolationModifier interpolation = InterpolationModifier::None;
  Type* type = nullptr;
  std::string_view name;
  std::string_view semantic;
  bool isArray = false;
  Expression* arraySize = nullptr;
  Expression* assignment = nullptr;
  Field* next = nullptr;
};

/// A struct declaration.
struct StructStmt : Statement {
  static const NodeType astType = NodeType::StructStmt;
  std::string_view name;
  Field* fields = nullptr;
  FunctionStmt* methods = nullptr;
};

/// A cbuffer or tbuffer declaration.
struct BufferStmt : Statement {
  static const NodeType astType = NodeType::BufferStmt;
  BufferType bufferType = BufferType::Cbuffer;
  std::string_view name;
  std::string_view registerName;
  Field* field = nullptr;
};

struct SamplerState : Expression {
  static const NodeType astType = NodeType::SamplerState;
  int numStateAssignments = 0;
  StateAssignment* stateAssignments = nullptr;
};

struct StateAssignment : Node {
  static const NodeType astType = NodeType::StateAssignment;
  std::string_view stateName;
  int d3dRenderState = 0;
  union {
    int intValue = 0;
    float floatValue;
  };
  std::string_view stringValue;
  StateAssignment* next = nullptr;
};

/// A prefix expression, such as a unary operator - or !.
struct PrefixExpr : Expression {
  static const NodeType astType = NodeType::PrefixExpr;
  Operator op = Operator::Undefined;
  Expression* expression = nullptr;
};

/// A postfix increment operator ++ or --.
struct IncrementExpr : Expression {
  static const NodeType astType = NodeType::IncrementExpr;
  Operator op = Operator::Undefined;
  Expression* variable = nullptr;
};

/// An array index expression such as foo[0].
struct ArrayExpr : Expression {
  static const NodeType astType = NodeType::ArrayExpr;
  Expression* array = nullptr;
  Expression* index = nullptr;
};

/// A member access expression such as foo.bar.
struct MemberExpr : Expression {
  static const NodeType astType = NodeType::MemberExpr;
  Expression* object = nullptr;
  Expression* member = nullptr;
};

/// A binary operator expression such as 1 + 2.
struct BinaryExpr : Expression {
  static const NodeType astType = NodeType::BinaryExpr;
  Operator op = Operator::Undefined;
  Expression* left = nullptr;
  Expression* right = nullptr;
};

/// A conditional expression such as foo ? bar : baz.
struct TernaryExpr : Expression {
  static const NodeType astType = NodeType::TernaryExpr;
  Expression* condition = nullptr;
  Expression* trueExpr = nullptr;
  Expression* falseExpr = nullptr;
};

/// A string value in an expression
struct StringExpr : Expression {
  static const NodeType astType = NodeType::StringExpr;
  std::string_view value;
};

/// A function call expression such as foo(1, 2, 3).
struct CallExpr : Expression {
  static const NodeType astType = NodeType::CallExpr;
  std::string_view name;
  Expression* arguments = nullptr;
};

/// A variable reference in an expression.
struct VariableExpr : Expression {
  static const NodeType astType = NodeType::VariableExpr;
  std::string_view name;
};

/// A literal value in an expression.
struct LiteralExpr : Expression {
  static const NodeType astType = NodeType::LiteralExpr;
  BaseType type = BaseType::Undefined;
  std::string_view value;
};

/// A cast expression such as (float)foo.
struct CastExpr : Expression {
  static const NodeType astType = NodeType::CastExpr;
  Type* type = nullptr;
  Expression* value = nullptr;
};

/// An assignment expression such as foo = 1.
struct AssignmentExpr : Expression {
  static const NodeType astType = NodeType::AssignmentExpr;
  Operator op = Operator::Undefined;
  Expression* variable = nullptr;
  Expression* value = nullptr;
};

/// A variable declaration statement.
struct VariableStmt : Statement {
  static const NodeType astType = NodeType::VariableStmt;
  std::string_view name;
  Type* type = nullptr;
  bool isArray = false;
  Expression* arraySize = nullptr;
  Expression* initializer = nullptr;
};

/// A function parameter declaration.
struct Parameter : Node {
  static const NodeType astType = NodeType::Parameter;
  std::string_view name;
  Type* type = nullptr;
  bool isArray = false;
  Expression* arraySize = nullptr;
  std::string_view semantic;
  Expression* initializer = nullptr;
  Parameter* next = nullptr;
};

/// A value used to initialize a struct.
struct StructInitializerExpr : Expression {
  static const NodeType astType = NodeType::StructInitializerExpr;
  StructStmt* structType = nullptr;
  Expression* fields = nullptr;
};

/// A value used to initialize an array, x = {1, 2}.
struct ArrayInitializerExpr : Expression {
  static const NodeType astType = NodeType::ArrayInitializerExpr;
  Expression* elements = nullptr;
};

/// A function declaration statement.
struct FunctionStmt : Statement {
  static const NodeType astType = NodeType::FunctionStmt;
  std::string_view name;
  Type* returnType = nullptr;
  Parameter* parameters = nullptr;
  std::string_view semantic;
  Block* body = nullptr;
};

/// An if statement.
struct IfStmt : Statement {
  static const NodeType astType = NodeType::IfStmt;
  Expression* condition = nullptr;
  Statement* body = nullptr;
  Statement* elseBody = nullptr;
};

/// A switch statement.
struct SwitchStmt : Statement {
  static const NodeType astType = NodeType::SwitchStmt;
  Expression* condition = nullptr;
  SwitchCase* cases = nullptr;
};

/// A switch case.
struct SwitchCase : Node {
  static const NodeType astType = NodeType::SwitchCase;
  bool isDefault = false; // true if this is the default case with no condition.
  Expression* condition = nullptr;
  Statement* body = nullptr;
  SwitchCase* next = nullptr;
};

/// A for loop statement.
struct ForStmt : Statement {
  static const NodeType astType = NodeType::ForStmt;
  Statement* initializer = nullptr;
  Expression* condition = nullptr;
  Statement* increment = nullptr;
  Statement* body = nullptr;
};

/// A do-while loop statement.
struct DoWhileStmt : Statement {
  static const NodeType astType = NodeType::DoWhileStmt;
  Statement* body = nullptr;
  Expression* condition = nullptr;
};

/// A while loop statement.
struct WhileStmt : Statement {
  static const NodeType astType = NodeType::WhileStmt;
  Expression* condition = nullptr;
  Statement* body = nullptr;
};

/// A discard statement.
struct DiscardStmt : Statement {
  static const NodeType astType = NodeType::DiscardStmt;
};

/// A return statement.
struct ReturnStmt : Statement {
  static const NodeType astType = NodeType::ReturnStmt;
  Expression* value = nullptr;
};

/// A break statement.
struct BreakStmt : Statement {
  static const NodeType astType = NodeType::BreakStmt;
};

/// A continue statement.
struct ContinueStmt : Statement {
  static const NodeType astType = NodeType::ContinueStmt;
};

/// A block of statements, encapsulated by { }.
struct Block : Statement {
  static const NodeType astType = NodeType::Block;
  Statement* statements = nullptr;
};

/// A statement that assigns a value to a variable.
struct AssignmentStmt : Statement {
  static const NodeType astType = NodeType::AssignmentStmt;
  /// The type of assignment operation such as = or +=.
  Operator op = Operator::Undefined;
  Expression* variable = nullptr;
  Expression* value = nullptr;
};

/// A statement that contains an expression.
struct ExpressionStmt : Statement {
  static const NodeType astType = NodeType::ExpressionStmt;
  Expression* expression = nullptr;
};

/// A statement that calls a function.
struct CallStmt : Statement {
  static const NodeType astType = NodeType::CallStmt;
  std::string_view name;
  Expression* arguments = nullptr;
};

/// A statement that declares a user type.
struct TypedefStmt : Statement {
  static const NodeType astType = NodeType::TypedefStmt;
  std::string_view name;
  Type* type = nullptr;
};

/// The root of the AST, containing a list of top-level statements.
struct Root : Node {
  static const NodeType astType = NodeType::Root;
  Statement* statements = nullptr;
};

} // namespace ast
