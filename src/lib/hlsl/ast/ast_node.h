#pragma once

#include <string_view>

#include "ast_type.h"
#include "attribute_type.h"
#include "interpolation_modifier.h"
#include "../token_type.h"

namespace hlsl {

struct AstNode {
  AstType nodeType = AstType::Undefined;
};

struct AstExpression : AstNode {
  
};

struct AstAttribute : AstNode {
  AttributeType attributeType = AttributeType::Unknown;
  AstExpression* expression = nullptr;
  AstAttribute* next = nullptr;
};

struct AstStatement : AstNode {
  AstAttribute* attributes = nullptr;
  AstStatement* next = nullptr;
};

struct AstStructField : AstNode {
  InterpolationModifier interpolation = InterpolationModifier::None;
  TokenType type;
  std::string_view name;
  AstStructField* next = nullptr;
};

struct AstStruct : AstStatement {
  static const AstType type = AstType::Struct;
  std::string_view name;
  AstStructField* field = nullptr;
};

struct AstRoot : AstNode {
  static const AstType type = AstType::Root;
  AstStatement* statement = nullptr;
};

} // namespace hlsl
