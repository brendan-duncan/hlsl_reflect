#pragma once

namespace hlsl {

enum class InterpolationModifier {
  None,
  Linear,
  Centroid,
  NoInterpolation,
  NoPerspective,
  Sample
};

} // namespace hlsl
