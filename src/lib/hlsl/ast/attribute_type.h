#pragma once

namespace hlsl {

enum class AttributeType {
  Unknown,
  Unroll,
  Branch,
  Flatten,
  NoFastMath
};

} // namespace hlsl
