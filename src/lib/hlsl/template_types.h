#pragma once

#include <set>

#include "token_type.h"

namespace hlsl {

// The following types take a template argument (ie. Vector<int 3>)
static const std::set<TokenType> templateTypes{
  TokenType::Buffer,
  TokenType::Matrix,
  TokenType::Vector,
};

inline bool isTemplateType(TokenType t) {
  return templateTypes.find(t) != templateTypes.end();
}

} // namespace hlsl
