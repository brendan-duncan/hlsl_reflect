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
#include "../token_type.h"

namespace hlsl {

struct AstAssignmentStmt;
struct AstAttribute;
struct AstBinaryExpr;
struct AstBlock;
struct AstBreakStmt;
struct AstBufferStmt;
struct AstCallExpr;
struct AstContinueStmt;
struct AstDiscardStmt;
struct AstDoWhileStmt;
struct AstExpression;
struct AstExpressionStmt;
struct AstForStmt;
struct AstFunctionStmt;
struct AstIfStmt;
struct AstLiteralExpr;
struct AstCastExpr;
struct AstAssignmentExpr;
struct AstNode;
struct AstParameter;
struct AstReturnStmt;
struct AstRoot;
struct AstSamplerState;
struct AstStateAssignment;
struct AstStatement;
struct AstStringExpr;
struct AstStructStmt;
struct AstField;
struct AstSwitchStmt;
struct AstSwitchCase;
struct AstTernaryExpr;
struct AstType;
struct AstTypedefStmt;
struct AstUnaryExpr;
struct AstVariableStmt;
struct AstVariableExpr;
struct AstWhileStmt;
struct AstCallStmt;

/// Call the callback for each node in the linked list
/// @tparam T AstNode with a next pointer, such as AstExpression
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
struct AstNode {
  AstNodeType nodeType = AstNodeType::Undefined;
};

/// Type declaration for a variable or function
struct AstType : AstNode {
  static const AstNodeType astType = AstNodeType::Type;
  BaseType baseType = BaseType::Undefined;
  SamplerType samplerType = SamplerType::Undefined; // Half or Float
  std::string_view name; // The name of the type if it's a user defined type
  bool array = false;
  AstExpression* arraySize = nullptr;
  uint32_t flags = TypeFlags::None;
  AddressSpace addressSpace = AddressSpace::Undefined;
};

/// Base class for all expressions
struct AstExpression : AstNode {
  AstExpression* next = nullptr;
};

/// Attributes augment a statement or expression with additional information.
struct AstAttribute : AstNode {
  static const AstNodeType astType = AstNodeType::Attribute;
  std::string_view name;
  AstExpression* argument = nullptr;
  AstAttribute* next = nullptr;
};

/// Base class for all statements
struct AstStatement : AstNode {
  AstAttribute* attributes = nullptr;
  AstStatement* next = nullptr;
};

/// A field member of a struct or buffer.
struct AstField : AstNode {
  static const AstNodeType astType = AstNodeType::Field;
  InterpolationModifier interpolation = InterpolationModifier::None;
  AstType* type = nullptr;
  std::string_view name;
  std::string_view semantic;
  bool isArray = false;
  AstExpression* arraySize = nullptr;
  AstExpression* assignment = nullptr;
  AstField* next = nullptr;
};

/// A struct declaration.
struct AstStructStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::StructStmt;
  std::string_view name;
  AstField* fields = nullptr;
};

/// A cbuffer or tbuffer declaration.
struct AstBufferStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::BufferStmt;
  BufferType bufferType;
  std::string_view name;
  std::string_view registerName;
  AstField* field = nullptr;
};

struct AstSamplerState : AstExpression {
  static const AstNodeType astType = AstNodeType::SamplerState;
  int numStateAssignments = 0;
  AstStateAssignment* stateAssignments = nullptr;
};

struct AstStateAssignment : AstNode {
  static const AstNodeType astType = AstNodeType::StateAssignment;
  std::string_view stateName;
  int d3dRenderState = 0;
  union {
    int intValue = 0;
    float floatValue;
  };
  std::string_view stringValue;
  AstStateAssignment* next = nullptr;
};

struct AstPrefixExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::PrefixExpr;
  Operator op = Operator::Undefined;
  AstExpression* expression = nullptr;
};

struct AstIncrementExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::IncrementExpr;
  Operator op = Operator::Undefined;
  AstExpression* variable = nullptr;
};

struct AstArrayExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::ArrayExpr;
  AstExpression* array = nullptr;
  AstExpression* index = nullptr;
};

struct AstMemberExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::MemberExpr;
  AstExpression* object = nullptr;
  AstExpression* member = nullptr;
};

struct AstBinaryExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::BinaryExpr;
  Operator op = Operator::Undefined;
  AstExpression* left = nullptr;
  AstExpression* right = nullptr;
};

struct AstTernaryExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::TernaryExpr;
  AstExpression* condition = nullptr;
  AstExpression* trueExpr = nullptr;
  AstExpression* falseExpr = nullptr;
};

struct AstStringExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::StringExpr;
  std::string_view value;
};

struct AstCallExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::CallExpr;
  std::string_view name;
  AstExpression* arguments = nullptr;
};

struct AstVariableExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::VariableExpr;
  std::string_view name;
};

struct AstLiteralExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::LiteralExpr;
  BaseType type = BaseType::Undefined;
  std::string_view value;
};

struct AstCastExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::CastExpr;
  AstType* type = nullptr;
  AstExpression* value = nullptr;
};

struct AstAssignmentExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::AssignmentExpr;
  Operator op = Operator::Undefined;
  AstExpression* variable = nullptr;
  AstExpression* value = nullptr;
};

struct AstVariableStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::VariableStmt;
  std::string_view name;
  AstType* type = nullptr;
  bool isArray = false;
  AstLiteralExpr* arraySize = nullptr;
  AstExpression* initializer = nullptr;
};

struct AstParameter : AstNode {
  static const AstNodeType astType = AstNodeType::Parameter;
  std::string_view name;
  AstType* type = nullptr;
  bool isArray = false;
  AstLiteralExpr* arraySize = nullptr;
  AstExpression* initializer = nullptr;
  AstParameter* next = nullptr;
};

struct AstStructInitializerExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::StructInitializerExpr;
  AstStructStmt* structType = nullptr;
  AstExpression* fields = nullptr;
};

struct AstArrayInitializerExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::ArrayInitializerExpr;
  AstExpression* elements = nullptr;
};

struct AstFunctionStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::FunctionStmt;
  std::string_view name;
  AstType* returnType = nullptr;
  AstParameter* parameters = nullptr;
  std::string_view semantic;
  AstBlock* body = nullptr;
};

struct AstIfStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::IfStmt;
  AstExpression* condition = nullptr;
  AstStatement* body = nullptr;
  AstStatement* elseBody = nullptr;
};

struct AstSwitchStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::SwitchStmt;
  AstExpression* condition = nullptr;
  AstSwitchCase* cases = nullptr;
};

struct AstSwitchCase : AstNode {
  static const AstNodeType astType = AstNodeType::SwitchCase;
  bool isDefault = false;
  AstExpression* condition = nullptr;
  AstStatement* body = nullptr;
  AstSwitchCase* next = nullptr;
};

struct AstForStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::ForStmt;
  AstStatement* initializer = nullptr;
  AstExpression* condition = nullptr;
  AstExpression* increment = nullptr;
  AstStatement* body = nullptr;
};

struct AstDoWhileStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::DoWhileStmt;
  AstStatement* body = nullptr;
  AstExpression* condition = nullptr;
};

struct AstWhileStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::WhileStmt;
  AstExpression* condition = nullptr;
  AstStatement* body = nullptr;
};

struct AstDiscardStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::DiscardStmt;
};

struct AstReturnStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::ReturnStmt;
  AstExpression* value = nullptr;
};

struct AstBreakStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::BreakStmt;
};

struct AstContinueStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::ContinueStmt;
};

struct AstBlock : AstStatement {
  static const AstNodeType astType = AstNodeType::Block;
  AstStatement* statements = nullptr;
};

struct AstAssignmentStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::AssignmentStmt;
  Operator op = Operator::Undefined;
  AstExpression* variable = nullptr;
  AstExpression* value = nullptr;
};

struct AstExpressionStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::ExpressionStmt;
  AstExpression* expression = nullptr;
};

struct AstCallStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::CallStmt;
  std::string_view name;
  AstExpression* arguments = nullptr;
};

struct AstTypedefStmt : AstStatement {
  static const AstNodeType astType = AstNodeType::TypedefStmt;
  std::string_view name;
  AstType* type = nullptr;
};

struct AstRoot : AstNode {
  static const AstNodeType astType = AstNodeType::Root;
  AstStatement* statements = nullptr;
};

} // namespace hlsl
