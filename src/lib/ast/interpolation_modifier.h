#pragma once

namespace ast {

enum class InterpolationModifier {
  None,
  Linear,
  Centroid,
  Nointerpolation,
  Noperspective,
  Sample
};

} // namespace ast
