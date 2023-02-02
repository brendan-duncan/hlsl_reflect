#pragma once

#include <string_view>

#include "address_space.h"
#include "ast_node_type.h"
#include "base_type.h"
#include "interpolation_modifier.h"
#include "operator.h"
#include "sampler_type.h"
#include "type_flags.h"
#include "../token_type.h"

namespace hlsl {

struct AstAssignment;
struct AstAttribute;
struct AstBinaryOperator;
struct AstBlock;
struct AstBreak;
struct AstBuffer;
struct AstCallExpr;
struct AstContinue;
struct AstDiscard;
struct AstDoWhile;
struct AstExpression;
struct AstExpressionStatement;
struct AstFor;
struct AstFunction;
struct AstIf;
struct AstLiteralExpr;
struct AstCastExpr;
struct AstNode;
struct AstParameter;
struct AstReturn;
struct AstRoot;
struct AstSamplerState;
struct AstStateAssignment;
struct AstStatement;
struct AstStringExpr;
struct AstStruct;
struct AstStructField;
struct AstSwitch;
struct AstSwitchCase;
struct AstTernaryOperator;
struct AstType;
struct AstUnaryOperator;
struct AstVariable;
struct AstVariableExpr;
struct AstWhile;
struct AstCall;

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
  TypeFlags flags = TypeFlags::None;
  AddressSpace addressSpace = AddressSpace::Undefined;
};

/// Base class for all expressions
struct AstExpression : AstNode {
  static const AstNodeType astType = AstNodeType::Expression;
  AstExpression* postfix = nullptr;
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

/// A member of a struct.
struct AstStructField : AstNode {
  static const AstNodeType astType = AstNodeType::StructField;
  InterpolationModifier interpolation = InterpolationModifier::None;
  AstType* type = nullptr;
  std::string_view name;
  AstStructField* next = nullptr;
};

struct AstStruct : AstStatement {
  static const AstNodeType astType = AstNodeType::Struct;
  std::string_view name;
  AstStructField* field = nullptr;
};

/// A field in a buffer.
struct AstBufferField : AstStatement {
  static const AstNodeType astType = AstNodeType::BufferField;
  AstType* type = nullptr;
  std::string_view name;
  std::string_view registerName;
  std::string_view semantic;
  AstExpression* assignment = nullptr;
  AstBuffer* buffer = nullptr;
  AstBufferField* next = nullptr;
};

/// A cbuffer or tbuffer declaration.
struct AstBuffer : AstStatement {
  static const AstNodeType astType = AstNodeType::Buffer;
  std::string_view name;
  std::string_view registerName;
  AstBufferField* field = nullptr;
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

struct AstUnaryOperator : AstExpression {
  static const AstNodeType astType = AstNodeType::UnaryOperator;
  Operator op = Operator::Undefined;
  AstExpression* expression = nullptr;
};

struct AstBinaryOperator : AstExpression {
  static const AstNodeType astType = AstNodeType::BinaryOperator;
  Operator op = Operator::Undefined;
  AstExpression* left = nullptr;
  AstExpression* right = nullptr;
};

struct AstTernaryOperator : AstExpression {
  static const AstNodeType astType = AstNodeType::TernaryOperator;
  AstExpression* condition = nullptr;
  AstExpression* trueExpression = nullptr;
  AstExpression* falseExpression = nullptr;
};

struct AstStringExpr : AstExpression {
  static const AstNodeType astType = AstNodeType::StringExpr;
  std::string_view value;
};

struct AstCallExpr : AstExpression {
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
  AstExpression* expression = nullptr;
};

struct AstVariable : AstStatement {
  static const AstNodeType astType = AstNodeType::Variable;
  std::string_view name;
  AstType* type = nullptr;
  AstExpression* initializer = nullptr;
};

struct AstParameter : AstStatement {
  static const AstNodeType astType = AstNodeType::Parameter;
  std::string_view name;
  AstType* type = nullptr;
  AstExpression* initializer = nullptr;
};

struct AstFunction : AstStatement {
  static const AstNodeType astType = AstNodeType::Function;
  std::string_view name;
  AstType* returnType = nullptr;
  AstParameter* parameters = nullptr;
  AstBlock* body = nullptr;
};

struct AstIf : AstStatement {
  static const AstNodeType astType = AstNodeType::If;
  AstExpression* condition = nullptr;
  AstStatement* body = nullptr;
  AstStatement* elseBody = nullptr;
};

struct AstSwitch : AstStatement {
  static const AstNodeType astType = AstNodeType::Switch;
  AstExpression* condition = nullptr;
  AstSwitchCase* cases = nullptr;
};

struct AstSwitchCase : AstStatement {
  static const AstNodeType astType = AstNodeType::SwitchCase;
  bool isDefault = false;
  AstExpression* condition = nullptr;
  AstStatement* body = nullptr;
  AstSwitchCase* next = nullptr;
};

struct AstFor : AstStatement {
  static const AstNodeType astType = AstNodeType::For;
  AstStatement* initializer = nullptr;
  AstExpression* condition = nullptr;
  AstExpression* increment = nullptr;
  AstStatement* body = nullptr;
};

struct AstDoWhile : AstStatement {
  static const AstNodeType astType = AstNodeType::DoWhile;
  AstStatement* body = nullptr;
  AstExpression* condition = nullptr;
};

struct AstWhile : AstStatement {
  static const AstNodeType astType = AstNodeType::While;
  AstExpression* condition = nullptr;
  AstStatement* body = nullptr;
};

struct AstDiscard : AstStatement {
  static const AstNodeType astType = AstNodeType::Discard;
};

struct AstReturn : AstStatement {
  static const AstNodeType astType = AstNodeType::Return;
  AstExpression* value = nullptr;
};

struct AstBreak : AstStatement {
  static const AstNodeType astType = AstNodeType::Break;
};

struct AstContinue : AstStatement {
  static const AstNodeType astType = AstNodeType::Continue;
};

struct AstBlock : AstStatement {
  static const AstNodeType astType = AstNodeType::Block;
  AstStatement* statements = nullptr;
};

struct AstAssignment : AstStatement {
  static const AstNodeType astType = AstNodeType::Assignment;
  Operator op = Operator::Undefined;
  AstExpression* variable = nullptr;
  AstExpression* value = nullptr;
};

struct AstExpressionStatement : AstStatement {
  static const AstNodeType astType = AstNodeType::ExpressionStatement;
  AstExpression* expression = nullptr;
};

struct AstCall : AstStatement {
  static const AstNodeType astType = AstNodeType::Call;
  std::string_view name;
  AstExpression* arguments = nullptr;
};

struct AstRoot : AstNode {
  static const AstNodeType astType = AstNodeType::Root;
  AstStatement* statements = nullptr;
};

} // namespace hlsl
