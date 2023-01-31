#pragma once

#include <string_view>

#include "ast_type.h"
#include "interpolation_modifier.h"
#include "../token_type.h"

namespace hlsl {

struct AstNode {
  AstType nodeType = AstType::Undefined;
};

struct AstExpression : AstNode {
  static const AstType astType = AstType::Expression;
};

struct AstArgument : AstNode {
  static const AstType astType = AstType::Argument;
  AstExpression* expression = nullptr;
  AstArgument* next = nullptr;
};

struct AstAttribute : AstNode {
  static const AstType astType = AstType::Attribute;
  std::string_view name;
  AstExpression* argument = nullptr;
  AstAttribute* next = nullptr;
};

struct AstStatement : AstNode {
  AstAttribute* attributes = nullptr;
  AstStatement* next = nullptr;
};

struct AstStructField : AstNode {
  static const AstType astType = AstType::StructField;
  InterpolationModifier interpolation = InterpolationModifier::None;
  TokenType type;
  std::string_view name;
  AstStructField* next = nullptr;
};

struct AstStruct : AstStatement {
  static const AstType astType = AstType::Struct;
  std::string_view name;
  AstStructField* field = nullptr;
};

struct AstRoot : AstNode {
  static const AstType astType = AstType::Root;
  AstStatement* statement = nullptr;
};

} // namespace hlsl
