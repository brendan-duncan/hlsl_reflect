#pragma once

namespace hlsl {

enum class StorageClass {
  Extern,
  NoInterpolation,
  Precise,
  Shared,
  GroupShared,
  Static,
  Uniform,
  Volatile
};

} // namespace hlsl
