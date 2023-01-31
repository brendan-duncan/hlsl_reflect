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

struct AstBuffer;
struct AstExpression;
struct AstStateAssignment;

/// Call the callback for each node in the linked list
/// @tparam T AstNode with a next pointer, such as AstArgument
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

struct AstArgument : AstNode {
  static const AstNodeType astType = AstNodeType::Argument;
  AstExpression* expression = nullptr;
  AstArgument* next = nullptr;
};

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

struct AstStructField : AstNode {
  static const AstNodeType astType = AstNodeType::StructField;
  InterpolationModifier interpolation = InterpolationModifier::None;
  TokenType type;
  std::string_view name;
  AstStructField* next = nullptr;
};

struct AstStruct : AstStatement {
  static const AstNodeType astType = AstNodeType::Struct;
  std::string_view name;
  AstStructField* field = nullptr;
};

struct AstDeclaration : AstStatement {
  static const AstNodeType astType = AstNodeType::Declaration;
  std::string_view name;
  AstType* type = nullptr;
  std::string_view registerName;
  std::string_view semantic;
  AstExpression* assignment = nullptr;
  AstBuffer* buffer = nullptr;  // The buffer that owns this declaration
  AstDeclaration* next = nullptr;
};

struct AstBuffer : AstStatement {
  static const AstNodeType astType = AstNodeType::Buffer;
  std::string_view name;
  std::string_view registerName;
  AstDeclaration* field = nullptr;
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

struct AstRoot : AstNode {
  static const AstNodeType astType = AstNodeType::Root;
  AstStatement* statement = nullptr;
};

} // namespace hlsl
