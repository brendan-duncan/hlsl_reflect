#pragma once

#include "../../ast/address_space.h"
#include "../../ast/base_type.h"
#include "../../ast/interpolation_modifier.h"
#include "../../ast/operator.h"
#include "../../ast/sampler_type.h"
#include "../../ast/storage_class.h"
#include "token.h"

namespace reader {
namespace hlsl {

inline ast::AddressSpace tokenToAddressSpace(Token t) {
  if (t.type() == TokenType::Identifier) {
    if (t.lexeme() == "constant") {
      return ast::AddressSpace::Constant;
    } else if (t.lexeme() == "device") {
      return ast::AddressSpace::Device;
    } else if (t.lexeme() == "thread") {
      return ast::AddressSpace::Thread;
    } else if (t.lexeme() == "shared") {
      return ast::AddressSpace::Shared;
    }
  } else if (t.type() == TokenType::Shared) {
    return ast::AddressSpace::Shared;
  }
  return ast::AddressSpace::Undefined;
}

inline ast::InterpolationModifier tokenTypeToInterpolationModifier(TokenType t) {
  switch (t) {
    case TokenType::Linear:
      return ast::InterpolationModifier::Linear;
    case TokenType::Centroid:
      return ast::InterpolationModifier::Centroid;
    case TokenType::Nointerpolation:
      return ast::InterpolationModifier::Nointerpolation;
    case TokenType::Noperspective:
      return ast::InterpolationModifier::Noperspective;
    case TokenType::Sample:
      return ast::InterpolationModifier::Sample;
    default:
      return ast::InterpolationModifier::None;
  }
}

inline bool isInterpolationModifier(TokenType type) {
  switch (type) {
    case TokenType::Linear:
    case TokenType::Centroid:
    case TokenType::Nointerpolation:
    case TokenType::Noperspective:
    case TokenType::Sample:
      return true;
    default:
      return false;
  }
}

inline ast::Operator tokenTypeToAssignmentOperatator(TokenType t) {
  switch (t) {
    case TokenType::Equal: return ast::Operator::Equal;
    case TokenType::MinusEqual: return ast::Operator::SubtractEqual;
    case TokenType::PlusEqual: return ast::Operator::AddEqual;
    case TokenType::StarEqual: return ast::Operator::MultiplyEqual;
    case TokenType::SlashEqual: return ast::Operator::DivideEqual;
    case TokenType::PercentEqual: return ast::Operator::ModuloEqual;
    case TokenType::LessLessEqual: return ast::Operator::LeftShiftEqual;
    case TokenType::GreaterGreaterEqual: return ast::Operator::RightShiftEqual;
    case TokenType::AmpersandEqual: return ast::Operator::AndEqual;
    case TokenType::PipeEqual: return ast::Operator::OrEqual;
    case TokenType::CaretEqual: return ast::Operator::XorEqual;
    default: return ast::Operator::Undefined;
  }
}

inline bool isAssignmentOperator(TokenType t) {
  return tokenTypeToAssignmentOperatator(t) != ast::Operator::Undefined;
}

inline ast::SamplerType tokenTypeToSamplerType(TokenType t) {
  if (t == TokenType::Float) {
    return ast::SamplerType::Float;
  } else if (t == TokenType::Half) {
    return ast::SamplerType::Half;
  }
  return ast::SamplerType::Undefined;
}

inline ast::StorageClass tokenTypeToStorageClass(TokenType type) {
  switch (type) {
    case TokenType::Extern:
      return ast::StorageClass::Extern;
    case TokenType::Nointerpolation:
      return ast::StorageClass::NoInterpolation;
    case TokenType::Precise:
      return ast::StorageClass::Precise;
    case TokenType::Shared:
      return ast::StorageClass::Shared;
    case TokenType::Groupshared:
      return ast::StorageClass::GroupShared;
    case TokenType::Static:
      return ast::StorageClass::Static;
    case TokenType::Uniform:
      return ast::StorageClass::Uniform;
    case TokenType::Volatile:
      return ast::StorageClass::Volatile;
    default:
      return ast::StorageClass::Undefined;
  }   
}

ast::BaseType tokenTypeToBaseType(TokenType t);

} // namespace hlsl
} // namespace reader
