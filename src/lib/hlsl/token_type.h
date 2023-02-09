#pragma once
// Generated by tools/gen_token_type.py
#include <iostream>
#include <stdint.h>
#include <string>
#include <string_view>

namespace hlsl {

enum class TokenType : uint32_t {
  Undefined,
  EndOfFile,
  IntLiteral,
  FloatLiteral,
  StringLiteral,
  Identifier,
  LeftParen,
  RightParen,
  LeftBracket,
  RightBracket,
  LeftBrace,
  RightBrace,
  Underscore,
  Dot,
  Comma,
  Colon,
  Semicolon,
  Plus,
  Minus,
  Star,
  Slash,
  Percent,
  Caret,
  Ampersand,
  Pipe,
  Tilde,
  Bang,
  Equal,
  Less,
  Greater,
  PlusEqual,
  MinusEqual,
  StarEqual,
  SlashEqual,
  PercentEqual,
  CaretEqual,
  AmpersandEqual,
  PipeEqual,
  LessLess,
  GreaterGreater,
  LessLessEqual,
  GreaterGreaterEqual,
  EqualEqual,
  BangEqual,
  LessEqual,
  GreaterEqual,
  AmpersandAmpersand,
  PipePipe,
  Question,
  QuestionQuestion,
  PlusPlus,
  MinusMinus,
  Hash,
  AppendStructuredBuffer,
  Asm,
  Asm_fragment,
  BlendState,
  Bool,
  Break,
  Buffer,
  ByteAddressBuffer,
  Case,
  Cbuffer,
  Centroid,
  Class,
  Column_major,
  Compile,
  Compile_fragment,
  CompileShader,
  Const,
  Continue,
  ComputeShader,
  ConsumeStructuredBuffer,
  Default,
  DepthStencilState,
  DepthStencilView,
  Discard,
  Do,
  Double,
  DomainShader,
  Dword,
  Else,
  Export,
  Extern,
  False,
  Float,
  For,
  Fxgroup,
  GeometryShader,
  Groupshared,
  Half,
  Hullshader,
  If,
  In,
  Inline,
  Inout,
  InputPatch,
  Int,
  Interface,
  Line,
  Lineadj,
  Linear,
  LineStream,
  Matrix,
  Min16float,
  Min10float,
  Min16int,
  Min12int,
  Min16uint,
  Namespace,
  Nointerpolation,
  Noperspective,
  Null,
  Out,
  OutputPatch,
  Packoffset,
  Pass,
  Pixelfragment,
  PixelShader,
  Point,
  PointStream,
  Precise,
  RasterizerState,
  RenderTargetView,
  Return,
  Register,
  Row_major,
  RWBuffer,
  RWByteAddressBuffer,
  RWStructuredBuffer,
  RWTexture1D,
  RWTexture1DArray,
  RWTexture2D,
  RWTexture2DArray,
  RWTexture3D,
  Sample,
  Sampler,
  Sampler2D,
  SamplerState,
  SamplerComparisonState,
  Shared,
  Snorm,
  Stateblock,
  Stateblock_state,
  Static,
  String,
  Struct,
  Switch,
  StructuredBuffer,
  Tbuffer,
  Technique,
  Technique10,
  Technique11,
  Texture,
  Texture1D,
  Texture1DArray,
  Texture2D,
  Texture2DArray,
  Texture2DMS,
  Texture2DMSArray,
  Texture3D,
  TextureCube,
  TextureCubeArray,
  True,
  Typedef,
  Triangle,
  Triangleadj,
  TriangleStream,
  Uint,
  Uniform,
  Unorm,
  Unsigned,
  Vector,
  Vertexfragment,
  VertexShader,
  Void,
  Volatile,
  While,
  Expression,
  UserDefined,
  Float1,
  Float1x1,
  Float1x2,
  Float1x3,
  Float1x4,
  Float2,
  Float2x1,
  Float2x2,
  Float2x3,
  Float2x4,
  Float3,
  Float3x1,
  Float3x2,
  Float3x3,
  Float3x4,
  Float4,
  Float4x1,
  Float4x2,
  Float4x3,
  Float4x4,
  Half1,
  Half1x1,
  Half1x2,
  Half1x3,
  Half1x4,
  Half2,
  Half2x1,
  Half2x2,
  Half2x3,
  Half2x4,
  Half3,
  Half3x1,
  Half3x2,
  Half3x3,
  Half3x4,
  Half4,
  Half4x1,
  Half4x2,
  Half4x3,
  Half4x4,
  Int1,
  Int1x1,
  Int1x2,
  Int1x3,
  Int1x4,
  Int2,
  Int2x1,
  Int2x2,
  Int2x3,
  Int2x4,
  Int3,
  Int3x1,
  Int3x2,
  Int3x3,
  Int3x4,
  Int4,
  Int4x1,
  Int4x2,
  Int4x3,
  Int4x4,
  Uint1,
  Uint1x1,
  Uint1x2,
  Uint1x3,
  Uint1x4,
  Uint2,
  Uint2x1,
  Uint2x2,
  Uint2x3,
  Uint2x4,
  Uint3,
  Uint3x1,
  Uint3x2,
  Uint3x3,
  Uint3x4,
  Uint4,
  Uint4x1,
  Uint4x2,
  Uint4x3,
  Uint4x4,
  Bool1,
  Bool1x1,
  Bool1x2,
  Bool1x3,
  Bool1x4,
  Bool2,
  Bool2x1,
  Bool2x2,
  Bool2x3,
  Bool2x4,
  Bool3,
  Bool3x1,
  Bool3x2,
  Bool3x3,
  Bool3x4,
  Bool4,
  Bool4x1,
  Bool4x2,
  Bool4x3,
  Bool4x4,
  Min10float1,
  Min10float1x1,
  Min10float1x2,
  Min10float1x3,
  Min10float1x4,
  Min10float2,
  Min10float2x1,
  Min10float2x2,
  Min10float2x3,
  Min10float2x4,
  Min10float3,
  Min10float3x1,
  Min10float3x2,
  Min10float3x3,
  Min10float3x4,
  Min10float4,
  Min10float4x1,
  Min10float4x2,
  Min10float4x3,
  Min10float4x4,
  Min16float1,
  Min16float1x1,
  Min16float1x2,
  Min16float1x3,
  Min16float1x4,
  Min16float2,
  Min16float2x1,
  Min16float2x2,
  Min16float2x3,
  Min16float2x4,
  Min16float3,
  Min16float3x1,
  Min16float3x2,
  Min16float3x3,
  Min16float3x4,
  Min16float4,
  Min16float4x1,
  Min16float4x2,
  Min16float4x3,
  Min16float4x4,
  Min12int1,
  Min12int1x1,
  Min12int1x2,
  Min12int1x3,
  Min12int1x4,
  Min12int2,
  Min12int2x1,
  Min12int2x2,
  Min12int2x3,
  Min12int2x4,
  Min12int3,
  Min12int3x1,
  Min12int3x2,
  Min12int3x3,
  Min12int3x4,
  Min12int4,
  Min12int4x1,
  Min12int4x2,
  Min12int4x3,
  Min12int4x4,
  Min16int1,
  Min16int1x1,
  Min16int1x2,
  Min16int1x3,
  Min16int1x4,
  Min16int2,
  Min16int2x1,
  Min16int2x2,
  Min16int2x3,
  Min16int2x4,
  Min16int3,
  Min16int3x1,
  Min16int3x2,
  Min16int3x3,
  Min16int3x4,
  Min16int4,
  Min16int4x1,
  Min16int4x2,
  Min16int4x3,
  Min16int4x4,
  Min16uint1,
  Min16uint1x1,
  Min16uint1x2,
  Min16uint1x3,
  Min16uint1x4,
  Min16uint2,
  Min16uint2x1,
  Min16uint2x2,
  Min16uint2x3,
  Min16uint2x4,
  Min16uint3,
  Min16uint3x1,
  Min16uint3x2,
  Min16uint3x3,
  Min16uint3x4,
  Min16uint4,
  Min16uint4x1,
  Min16uint4x2,
  Min16uint4x3,
  Min16uint4x4,
};

inline std::ostream& operator<<(std::ostream& os, const TokenType& obj) {
  os << static_cast<std::underlying_type<TokenType>::type>(obj);
  return os;
}

const std::string& tokenTypeToString(TokenType t);

TokenType findTokenType(const std::string_view& s);

} // namespace hlsl
