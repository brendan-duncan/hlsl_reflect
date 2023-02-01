#include "storage_class.h"

namespace hlsl {

StorageClass tokenTypeToStorageClass(TokenType type) {
  switch (type) {
    case TokenType::Extern:
      return StorageClass::Extern;
    case TokenType::Nointerpolation:
      return StorageClass::NoInterpolation;
    case TokenType::Precise:
      return StorageClass::Precise;
    case TokenType::Shared:
      return StorageClass::Shared;
    case TokenType::Groupshared:
      return StorageClass::GroupShared;
    case TokenType::Static:
      return StorageClass::Static;
    case TokenType::Uniform:
      return StorageClass::Uniform;
    case TokenType::Volatile:
      return StorageClass::Volatile;
    default:
      return StorageClass::Undefined;
  }   
}

} // namespace hlsl 