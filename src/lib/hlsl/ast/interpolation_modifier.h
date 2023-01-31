#pragma once

#include "../token_type.h"

namespace hlsl {

enum class InterpolationModifier {
  None,
  Linear,
  Centroid,
  Nointerpolation,
  Noperspective,
  Sample
};

inline InterpolationModifier tokenTypeToInterpolationModifier(TokenType t) {
  switch (t) {
    case TokenType::Linear:
      return InterpolationModifier::Linear;
    case TokenType::Centroid:
      return InterpolationModifier::Centroid;
    case TokenType::Nointerpolation:
      return InterpolationModifier::Nointerpolation;
    case TokenType::Noperspective:
      return InterpolationModifier::Noperspective;
    case TokenType::Sample:
      return InterpolationModifier::Sample;
    default:
      return InterpolationModifier::None;
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

} // namespace hlsl
