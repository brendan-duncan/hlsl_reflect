#pragma once

#include "../token_type.h"

namespace hlsl {

enum class StorageClass {
  Undefined,

  Extern,
  NoInterpolation,
  Precise,
  Shared,
  GroupShared,
  Static,
  Uniform,
  Volatile
};

StorageClass tokenTypeToStorageClass(TokenType type);

} // namespace hlsl
