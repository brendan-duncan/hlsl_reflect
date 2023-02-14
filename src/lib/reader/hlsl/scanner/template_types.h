#pragma once

#include <set>

#include "../token_type.h"

namespace reader {
namespace hlsl {

inline bool isTemplateType(TokenType t) {
  switch (t) {
    case TokenType::Buffer:
    case TokenType::Matrix:
    case TokenType::Vector:
    case TokenType::Texture2D:
    case TokenType::StructuredBuffer:
    case TokenType::RWStructuredBuffer:
    case TokenType::RWTexture2D:
    case TokenType::RWBuffer:
    case TokenType::RWByteAddressBuffer:
    case TokenType::ByteAddressBuffer:
    case TokenType::AppendStructuredBuffer:
    case TokenType::ConsumeStructuredBuffer:
    case TokenType::Texture2DArray:
    case TokenType::Texture3D:
    case TokenType::TextureCube:
    case TokenType::TextureCubeArray:
    case TokenType::Texture2DMS:
    case TokenType::Texture2DMSArray:
    case TokenType::RWTexture2DArray:
    case TokenType::RWTexture3D:
      return true;
    default:
      return false;  
  }
}

} // namespace hlsl
} // namespace reader
