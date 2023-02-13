#pragma once

namespace ast {

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

} // namespace ast
