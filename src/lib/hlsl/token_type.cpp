// Generated from gen_token_type.py
#include <array>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <variant>

#include "literal.h"
#include "token_type.h"

namespace hlsl {

// Used for diagnostics
static const std::map<TokenType, std::string> _tokenTypeToString{
  {TokenType::Undefined, "Undefined"},
  {TokenType::EndOfFile, "EndOfFile"},
  {TokenType::IntLiteral, "IntLiteral"},
  {TokenType::FloatLiteral, "FloatLiteral"},
  {TokenType::Identifier, "Identifier"},
  {TokenType::LeftParen, "LeftParen"},
  {TokenType::RightParen, "RightParen"},
  {TokenType::LeftBracket, "LeftBracket"},
  {TokenType::RightBracket, "RightBracket"},
  {TokenType::LeftBrace, "LeftBrace"},
  {TokenType::RightBrace, "RightBrace"},
  {TokenType::Dot, "Dot"},
  {TokenType::Comma, "Comma"},
  {TokenType::Colon, "Colon"},
  {TokenType::Semicolon, "Semicolon"},
  {TokenType::Plus, "Plus"},
  {TokenType::Minus, "Minus"},
  {TokenType::Star, "Star"},
  {TokenType::Slash, "Slash"},
  {TokenType::Percent, "Percent"},
  {TokenType::Caret, "Caret"},
  {TokenType::Ampersand, "Ampersand"},
  {TokenType::Pipe, "Pipe"},
  {TokenType::Tilde, "Tilde"},
  {TokenType::Bang, "Bang"},
  {TokenType::Equal, "Equal"},
  {TokenType::Less, "Less"},
  {TokenType::Greater, "Greater"},
  {TokenType::PlusEqual, "PlusEqual"},
  {TokenType::MinusEqual, "MinusEqual"},
  {TokenType::StarEqual, "StarEqual"},
  {TokenType::SlashEqual, "SlashEqual"},
  {TokenType::PercentEqual, "PercentEqual"},
  {TokenType::CaretEqual, "CaretEqual"},
  {TokenType::AmpersandEqual, "AmpersandEqual"},
  {TokenType::PipeEqual, "PipeEqual"},
  {TokenType::LessLess, "LessLess"},
  {TokenType::GreaterGreater, "GreaterGreater"},
  {TokenType::LessLessEqual, "LessLessEqual"},
  {TokenType::GreaterGreaterEqual, "GreaterGreaterEqual"},
  {TokenType::EqualEqual, "EqualEqual"},
  {TokenType::BangEqual, "BangEqual"},
  {TokenType::LessEqual, "LessEqual"},
  {TokenType::GreaterEqual, "GreaterEqual"},
  {TokenType::AmpersandAmpersand, "AmpersandAmpersand"},
  {TokenType::PipePipe, "PipePipe"},
  {TokenType::Question, "Question"},
  {TokenType::QuestionQuestion, "QuestionQuestion"},
  {TokenType::PlusPlus, "PlusPlus"},
  {TokenType::MinusMinus, "MinusMinus"},
  {TokenType::Hash, "Hash"},
  {TokenType::AppendStructuredBuffer, "AppendStructuredBuffer"},
  {TokenType::Asm, "Asm"},
  {TokenType::Asm_fragment, "Asm_fragment"},
  {TokenType::BlendState, "BlendState"},
  {TokenType::Bool, "Bool"},
  {TokenType::Break, "Break"},
  {TokenType::Buffer, "Buffer"},
  {TokenType::ByteAddressBuffer, "ByteAddressBuffer"},
  {TokenType::Case, "Case"},
  {TokenType::Cbuffer, "Cbuffer"},
  {TokenType::Centroid, "Centroid"},
  {TokenType::Class, "Class"},
  {TokenType::Column_major, "Column_major"},
  {TokenType::Compile, "Compile"},
  {TokenType::Compile_fragment, "Compile_fragment"},
  {TokenType::CompileShader, "CompileShader"},
  {TokenType::Const, "Const"},
  {TokenType::Continue, "Continue"},
  {TokenType::ComputeShader, "ComputeShader"},
  {TokenType::ConsumeStructuredBuffer, "ConsumeStructuredBuffer"},
  {TokenType::Default, "Default"},
  {TokenType::DepthStencilState, "DepthStencilState"},
  {TokenType::DepthStencilView, "DepthStencilView"},
  {TokenType::Discard, "Discard"},
  {TokenType::Do, "Do"},
  {TokenType::Double, "Double"},
  {TokenType::DomainShader, "DomainShader"},
  {TokenType::Dword, "Dword"},
  {TokenType::Else, "Else"},
  {TokenType::Export, "Export"},
  {TokenType::Extern, "Extern"},
  {TokenType::False, "False"},
  {TokenType::Float, "Float"},
  {TokenType::For, "For"},
  {TokenType::Fxgroup, "Fxgroup"},
  {TokenType::GeometryShader, "GeometryShader"},
  {TokenType::Groupshared, "Groupshared"},
  {TokenType::Half, "Half"},
  {TokenType::Hullshader, "Hullshader"},
  {TokenType::If, "If"},
  {TokenType::In, "In"},
  {TokenType::Inline, "Inline"},
  {TokenType::Inout, "Inout"},
  {TokenType::InputPatch, "InputPatch"},
  {TokenType::Int, "Int"},
  {TokenType::Interface, "Interface"},
  {TokenType::Line, "Line"},
  {TokenType::Lineadj, "Lineadj"},
  {TokenType::Linear, "Linear"},
  {TokenType::LineStream, "LineStream"},
  {TokenType::Matrix, "Matrix"},
  {TokenType::Min16float, "Min16float"},
  {TokenType::Min10float, "Min10float"},
  {TokenType::Min16int, "Min16int"},
  {TokenType::Min12int, "Min12int"},
  {TokenType::Min16uint, "Min16uint"},
  {TokenType::Namespace, "Namespace"},
  {TokenType::Nointerpolation, "Nointerpolation"},
  {TokenType::Noperspective, "Noperspective"},
  {TokenType::Null, "Null"},
  {TokenType::Out, "Out"},
  {TokenType::OutputPatch, "OutputPatch"},
  {TokenType::Packoffset, "Packoffset"},
  {TokenType::Pass, "Pass"},
  {TokenType::Pixelfragment, "Pixelfragment"},
  {TokenType::PixelShader, "PixelShader"},
  {TokenType::Point, "Point"},
  {TokenType::PointStream, "PointStream"},
  {TokenType::Precise, "Precise"},
  {TokenType::RasterizerState, "RasterizerState"},
  {TokenType::RenderTargetView, "RenderTargetView"},
  {TokenType::Return, "Return"},
  {TokenType::Register, "Register"},
  {TokenType::Row_major, "Row_major"},
  {TokenType::RWBuffer, "RWBuffer"},
  {TokenType::RWByteAddressBuffer, "RWByteAddressBuffer"},
  {TokenType::RWStructuredBuffer, "RWStructuredBuffer"},
  {TokenType::RWTexture1D, "RWTexture1D"},
  {TokenType::RWTexture1DArray, "RWTexture1DArray"},
  {TokenType::RWTexture2D, "RWTexture2D"},
  {TokenType::RWTexture2DArray, "RWTexture2DArray"},
  {TokenType::RWTexture3D, "RWTexture3D"},
  {TokenType::Sample, "Sample"},
  {TokenType::Sampler, "Sampler"},
  {TokenType::SamplerState, "SamplerState"},
  {TokenType::SamplerComparisonState, "SamplerComparisonState"},
  {TokenType::Shared, "Shared"},
  {TokenType::Snorm, "Snorm"},
  {TokenType::Stateblock, "Stateblock"},
  {TokenType::Stateblock_state, "Stateblock_state"},
  {TokenType::Static, "Static"},
  {TokenType::String, "String"},
  {TokenType::Struct, "Struct"},
  {TokenType::Switch, "Switch"},
  {TokenType::StructuredBuffer, "StructuredBuffer"},
  {TokenType::Tbuffer, "Tbuffer"},
  {TokenType::Technique, "Technique"},
  {TokenType::Technique10, "Technique10"},
  {TokenType::Technique11, "Technique11"},
  {TokenType::Texture, "Texture"},
  {TokenType::Texture1D, "Texture1D"},
  {TokenType::Texture1DArray, "Texture1DArray"},
  {TokenType::Texture2D, "Texture2D"},
  {TokenType::Texture2DArray, "Texture2DArray"},
  {TokenType::Texture2DMS, "Texture2DMS"},
  {TokenType::Texture2DMSArray, "Texture2DMSArray"},
  {TokenType::Texture3D, "Texture3D"},
  {TokenType::TextureCube, "TextureCube"},
  {TokenType::TextureCubeArray, "TextureCubeArray"},
  {TokenType::True, "True"},
  {TokenType::Typedef, "Typedef"},
  {TokenType::Triangle, "Triangle"},
  {TokenType::Triangleadj, "Triangleadj"},
  {TokenType::TriangleStream, "TriangleStream"},
  {TokenType::Uint, "Uint"},
  {TokenType::Uniform, "Uniform"},
  {TokenType::Unorm, "Unorm"},
  {TokenType::Unsigned, "Unsigned"},
  {TokenType::Vector, "Vector"},
  {TokenType::Vertexfragment, "Vertexfragment"},
  {TokenType::VertexShader, "VertexShader"},
  {TokenType::Void, "Void"},
  {TokenType::Volatile, "Volatile"},
  {TokenType::While, "While"},
  {TokenType::Expression, "Expression"},
  {TokenType::Float1, "Float1"},
  {TokenType::Float1x1, "Float1x1"},
  {TokenType::Float1x2, "Float1x2"},
  {TokenType::Float1x3, "Float1x3"},
  {TokenType::Float1x4, "Float1x4"},
  {TokenType::Float2, "Float2"},
  {TokenType::Float2x1, "Float2x1"},
  {TokenType::Float2x2, "Float2x2"},
  {TokenType::Float2x3, "Float2x3"},
  {TokenType::Float2x4, "Float2x4"},
  {TokenType::Float3, "Float3"},
  {TokenType::Float3x1, "Float3x1"},
  {TokenType::Float3x2, "Float3x2"},
  {TokenType::Float3x3, "Float3x3"},
  {TokenType::Float3x4, "Float3x4"},
  {TokenType::Float4, "Float4"},
  {TokenType::Float4x1, "Float4x1"},
  {TokenType::Float4x2, "Float4x2"},
  {TokenType::Float4x3, "Float4x3"},
  {TokenType::Float4x4, "Float4x4"},
  {TokenType::Int1, "Int1"},
  {TokenType::Int1x1, "Int1x1"},
  {TokenType::Int1x2, "Int1x2"},
  {TokenType::Int1x3, "Int1x3"},
  {TokenType::Int1x4, "Int1x4"},
  {TokenType::Int2, "Int2"},
  {TokenType::Int2x1, "Int2x1"},
  {TokenType::Int2x2, "Int2x2"},
  {TokenType::Int2x3, "Int2x3"},
  {TokenType::Int2x4, "Int2x4"},
  {TokenType::Int3, "Int3"},
  {TokenType::Int3x1, "Int3x1"},
  {TokenType::Int3x2, "Int3x2"},
  {TokenType::Int3x3, "Int3x3"},
  {TokenType::Int3x4, "Int3x4"},
  {TokenType::Int4, "Int4"},
  {TokenType::Int4x1, "Int4x1"},
  {TokenType::Int4x2, "Int4x2"},
  {TokenType::Int4x3, "Int4x3"},
  {TokenType::Int4x4, "Int4x4"},
  {TokenType::Uint1, "Uint1"},
  {TokenType::Uint1x1, "Uint1x1"},
  {TokenType::Uint1x2, "Uint1x2"},
  {TokenType::Uint1x3, "Uint1x3"},
  {TokenType::Uint1x4, "Uint1x4"},
  {TokenType::Uint2, "Uint2"},
  {TokenType::Uint2x1, "Uint2x1"},
  {TokenType::Uint2x2, "Uint2x2"},
  {TokenType::Uint2x3, "Uint2x3"},
  {TokenType::Uint2x4, "Uint2x4"},
  {TokenType::Uint3, "Uint3"},
  {TokenType::Uint3x1, "Uint3x1"},
  {TokenType::Uint3x2, "Uint3x2"},
  {TokenType::Uint3x3, "Uint3x3"},
  {TokenType::Uint3x4, "Uint3x4"},
  {TokenType::Uint4, "Uint4"},
  {TokenType::Uint4x1, "Uint4x1"},
  {TokenType::Uint4x2, "Uint4x2"},
  {TokenType::Uint4x3, "Uint4x3"},
  {TokenType::Uint4x4, "Uint4x4"},
  {TokenType::Bool1, "Bool1"},
  {TokenType::Bool1x1, "Bool1x1"},
  {TokenType::Bool1x2, "Bool1x2"},
  {TokenType::Bool1x3, "Bool1x3"},
  {TokenType::Bool1x4, "Bool1x4"},
  {TokenType::Bool2, "Bool2"},
  {TokenType::Bool2x1, "Bool2x1"},
  {TokenType::Bool2x2, "Bool2x2"},
  {TokenType::Bool2x3, "Bool2x3"},
  {TokenType::Bool2x4, "Bool2x4"},
  {TokenType::Bool3, "Bool3"},
  {TokenType::Bool3x1, "Bool3x1"},
  {TokenType::Bool3x2, "Bool3x2"},
  {TokenType::Bool3x3, "Bool3x3"},
  {TokenType::Bool3x4, "Bool3x4"},
  {TokenType::Bool4, "Bool4"},
  {TokenType::Bool4x1, "Bool4x1"},
  {TokenType::Bool4x2, "Bool4x2"},
  {TokenType::Bool4x3, "Bool4x3"},
  {TokenType::Bool4x4, "Bool4x4"},
  {TokenType::Min10float1, "Min10float1"},
  {TokenType::Min10float1x1, "Min10float1x1"},
  {TokenType::Min10float1x2, "Min10float1x2"},
  {TokenType::Min10float1x3, "Min10float1x3"},
  {TokenType::Min10float1x4, "Min10float1x4"},
  {TokenType::Min10float2, "Min10float2"},
  {TokenType::Min10float2x1, "Min10float2x1"},
  {TokenType::Min10float2x2, "Min10float2x2"},
  {TokenType::Min10float2x3, "Min10float2x3"},
  {TokenType::Min10float2x4, "Min10float2x4"},
  {TokenType::Min10float3, "Min10float3"},
  {TokenType::Min10float3x1, "Min10float3x1"},
  {TokenType::Min10float3x2, "Min10float3x2"},
  {TokenType::Min10float3x3, "Min10float3x3"},
  {TokenType::Min10float3x4, "Min10float3x4"},
  {TokenType::Min10float4, "Min10float4"},
  {TokenType::Min10float4x1, "Min10float4x1"},
  {TokenType::Min10float4x2, "Min10float4x2"},
  {TokenType::Min10float4x3, "Min10float4x3"},
  {TokenType::Min10float4x4, "Min10float4x4"},
  {TokenType::Min16float1, "Min16float1"},
  {TokenType::Min16float1x1, "Min16float1x1"},
  {TokenType::Min16float1x2, "Min16float1x2"},
  {TokenType::Min16float1x3, "Min16float1x3"},
  {TokenType::Min16float1x4, "Min16float1x4"},
  {TokenType::Min16float2, "Min16float2"},
  {TokenType::Min16float2x1, "Min16float2x1"},
  {TokenType::Min16float2x2, "Min16float2x2"},
  {TokenType::Min16float2x3, "Min16float2x3"},
  {TokenType::Min16float2x4, "Min16float2x4"},
  {TokenType::Min16float3, "Min16float3"},
  {TokenType::Min16float3x1, "Min16float3x1"},
  {TokenType::Min16float3x2, "Min16float3x2"},
  {TokenType::Min16float3x3, "Min16float3x3"},
  {TokenType::Min16float3x4, "Min16float3x4"},
  {TokenType::Min16float4, "Min16float4"},
  {TokenType::Min16float4x1, "Min16float4x1"},
  {TokenType::Min16float4x2, "Min16float4x2"},
  {TokenType::Min16float4x3, "Min16float4x3"},
  {TokenType::Min16float4x4, "Min16float4x4"},
  {TokenType::Min12int1, "Min12int1"},
  {TokenType::Min12int1x1, "Min12int1x1"},
  {TokenType::Min12int1x2, "Min12int1x2"},
  {TokenType::Min12int1x3, "Min12int1x3"},
  {TokenType::Min12int1x4, "Min12int1x4"},
  {TokenType::Min12int2, "Min12int2"},
  {TokenType::Min12int2x1, "Min12int2x1"},
  {TokenType::Min12int2x2, "Min12int2x2"},
  {TokenType::Min12int2x3, "Min12int2x3"},
  {TokenType::Min12int2x4, "Min12int2x4"},
  {TokenType::Min12int3, "Min12int3"},
  {TokenType::Min12int3x1, "Min12int3x1"},
  {TokenType::Min12int3x2, "Min12int3x2"},
  {TokenType::Min12int3x3, "Min12int3x3"},
  {TokenType::Min12int3x4, "Min12int3x4"},
  {TokenType::Min12int4, "Min12int4"},
  {TokenType::Min12int4x1, "Min12int4x1"},
  {TokenType::Min12int4x2, "Min12int4x2"},
  {TokenType::Min12int4x3, "Min12int4x3"},
  {TokenType::Min12int4x4, "Min12int4x4"},
  {TokenType::Min16int1, "Min16int1"},
  {TokenType::Min16int1x1, "Min16int1x1"},
  {TokenType::Min16int1x2, "Min16int1x2"},
  {TokenType::Min16int1x3, "Min16int1x3"},
  {TokenType::Min16int1x4, "Min16int1x4"},
  {TokenType::Min16int2, "Min16int2"},
  {TokenType::Min16int2x1, "Min16int2x1"},
  {TokenType::Min16int2x2, "Min16int2x2"},
  {TokenType::Min16int2x3, "Min16int2x3"},
  {TokenType::Min16int2x4, "Min16int2x4"},
  {TokenType::Min16int3, "Min16int3"},
  {TokenType::Min16int3x1, "Min16int3x1"},
  {TokenType::Min16int3x2, "Min16int3x2"},
  {TokenType::Min16int3x3, "Min16int3x3"},
  {TokenType::Min16int3x4, "Min16int3x4"},
  {TokenType::Min16int4, "Min16int4"},
  {TokenType::Min16int4x1, "Min16int4x1"},
  {TokenType::Min16int4x2, "Min16int4x2"},
  {TokenType::Min16int4x3, "Min16int4x3"},
  {TokenType::Min16int4x4, "Min16int4x4"},
  {TokenType::Min16uint1, "Min16uint1"},
  {TokenType::Min16uint1x1, "Min16uint1x1"},
  {TokenType::Min16uint1x2, "Min16uint1x2"},
  {TokenType::Min16uint1x3, "Min16uint1x3"},
  {TokenType::Min16uint1x4, "Min16uint1x4"},
  {TokenType::Min16uint2, "Min16uint2"},
  {TokenType::Min16uint2x1, "Min16uint2x1"},
  {TokenType::Min16uint2x2, "Min16uint2x2"},
  {TokenType::Min16uint2x3, "Min16uint2x3"},
  {TokenType::Min16uint2x4, "Min16uint2x4"},
  {TokenType::Min16uint3, "Min16uint3"},
  {TokenType::Min16uint3x1, "Min16uint3x1"},
  {TokenType::Min16uint3x2, "Min16uint3x2"},
  {TokenType::Min16uint3x3, "Min16uint3x3"},
  {TokenType::Min16uint3x4, "Min16uint3x4"},
  {TokenType::Min16uint4, "Min16uint4"},
  {TokenType::Min16uint4x1, "Min16uint4x1"},
  {TokenType::Min16uint4x2, "Min16uint4x2"},
  {TokenType::Min16uint4x3, "Min16uint4x3"},
  {TokenType::Min16uint4x4, "Min16uint4x4"},
}; // _tokenTypeToString

static const std::map<std::string_view, TokenType> tokenDefs {
  {"(", TokenType::LeftParen},
  {")", TokenType::RightParen},
  {"[", TokenType::LeftBracket},
  {"]", TokenType::RightBracket},
  {"{", TokenType::LeftBrace},
  {"}", TokenType::RightBrace},
  {".", TokenType::Dot},
  {",", TokenType::Comma},
  {":", TokenType::Colon},
  {";", TokenType::Semicolon},
  {"+", TokenType::Plus},
  {"-", TokenType::Minus},
  {"*", TokenType::Star},
  {"/", TokenType::Slash},
  {"%", TokenType::Percent},
  {"^", TokenType::Caret},
  {"&", TokenType::Ampersand},
  {"|", TokenType::Pipe},
  {"~", TokenType::Tilde},
  {"!", TokenType::Bang},
  {"=", TokenType::Equal},
  {"<", TokenType::Less},
  {">", TokenType::Greater},
  {"+=", TokenType::PlusEqual},
  {"-=", TokenType::MinusEqual},
  {"*=", TokenType::StarEqual},
  {"/=", TokenType::SlashEqual},
  {"%=", TokenType::PercentEqual},
  {"^=", TokenType::CaretEqual},
  {"&=", TokenType::AmpersandEqual},
  {"|=", TokenType::PipeEqual},
  {"<<", TokenType::LessLess},
  {">>", TokenType::GreaterGreater},
  {"<<=", TokenType::LessLessEqual},
  {">>=", TokenType::GreaterGreaterEqual},
  {"==", TokenType::EqualEqual},
  {"!=", TokenType::BangEqual},
  {"<=", TokenType::LessEqual},
  {">=", TokenType::GreaterEqual},
  {"&&", TokenType::AmpersandAmpersand},
  {"||", TokenType::PipePipe},
  {"?", TokenType::Question},
  {"??", TokenType::QuestionQuestion},
  {"++", TokenType::PlusPlus},
  {"--", TokenType::MinusMinus},
  {"#", TokenType::Hash},
  {"AppendStructuredBuffer", TokenType::AppendStructuredBuffer},
  {"asm", TokenType::Asm},
  {"asm_fragment", TokenType::Asm_fragment},
  {"BlendState", TokenType::BlendState},
  {"bool", TokenType::Bool},
  {"break", TokenType::Break},
  {"Buffer", TokenType::Buffer},
  {"ByteAddressBuffer", TokenType::ByteAddressBuffer},
  {"case", TokenType::Case},
  {"cbuffer", TokenType::Cbuffer},
  {"centroid", TokenType::Centroid},
  {"class", TokenType::Class},
  {"column_major", TokenType::Column_major},
  {"compile", TokenType::Compile},
  {"compile_fragment", TokenType::Compile_fragment},
  {"CompileShader", TokenType::CompileShader},
  {"const", TokenType::Const},
  {"continue", TokenType::Continue},
  {"ComputeShader", TokenType::ComputeShader},
  {"ConsumeStructuredBuffer", TokenType::ConsumeStructuredBuffer},
  {"default", TokenType::Default},
  {"DepthStencilState", TokenType::DepthStencilState},
  {"DepthStencilView", TokenType::DepthStencilView},
  {"discard", TokenType::Discard},
  {"do", TokenType::Do},
  {"double", TokenType::Double},
  {"DomainShader", TokenType::DomainShader},
  {"dword", TokenType::Dword},
  {"else", TokenType::Else},
  {"export", TokenType::Export},
  {"extern", TokenType::Extern},
  {"false", TokenType::False},
  {"float", TokenType::Float},
  {"for", TokenType::For},
  {"fxgroup", TokenType::Fxgroup},
  {"GeometryShader", TokenType::GeometryShader},
  {"groupshared", TokenType::Groupshared},
  {"half", TokenType::Half},
  {"Hullshader", TokenType::Hullshader},
  {"if", TokenType::If},
  {"in", TokenType::In},
  {"inline", TokenType::Inline},
  {"inout", TokenType::Inout},
  {"InputPatch", TokenType::InputPatch},
  {"int", TokenType::Int},
  {"interface", TokenType::Interface},
  {"line", TokenType::Line},
  {"lineadj", TokenType::Lineadj},
  {"linear", TokenType::Linear},
  {"LineStream", TokenType::LineStream},
  {"matrix", TokenType::Matrix},
  {"min16float", TokenType::Min16float},
  {"min10float", TokenType::Min10float},
  {"min16int", TokenType::Min16int},
  {"min12int", TokenType::Min12int},
  {"min16uint", TokenType::Min16uint},
  {"namespace", TokenType::Namespace},
  {"nointerpolation", TokenType::Nointerpolation},
  {"noperspective", TokenType::Noperspective},
  {"NULL", TokenType::Null},
  {"out", TokenType::Out},
  {"OutputPatch", TokenType::OutputPatch},
  {"packoffset", TokenType::Packoffset},
  {"pass", TokenType::Pass},
  {"pixelfragment", TokenType::Pixelfragment},
  {"PixelShader", TokenType::PixelShader},
  {"point", TokenType::Point},
  {"PointStream", TokenType::PointStream},
  {"precise", TokenType::Precise},
  {"RasterizerState", TokenType::RasterizerState},
  {"RenderTargetView", TokenType::RenderTargetView},
  {"return", TokenType::Return},
  {"register", TokenType::Register},
  {"row_major", TokenType::Row_major},
  {"RWBuffer", TokenType::RWBuffer},
  {"RWByteAddressBuffer", TokenType::RWByteAddressBuffer},
  {"RWStructuredBuffer", TokenType::RWStructuredBuffer},
  {"RWTexture1D", TokenType::RWTexture1D},
  {"RWTexture1DArray", TokenType::RWTexture1DArray},
  {"RWTexture2D", TokenType::RWTexture2D},
  {"RWTexture2DArray", TokenType::RWTexture2DArray},
  {"RWTexture3D", TokenType::RWTexture3D},
  {"sample", TokenType::Sample},
  {"sampler", TokenType::Sampler},
  {"SamplerState", TokenType::SamplerState},
  {"SamplerComparisonState", TokenType::SamplerComparisonState},
  {"shared", TokenType::Shared},
  {"snorm", TokenType::Snorm},
  {"stateblock", TokenType::Stateblock},
  {"stateblock_state", TokenType::Stateblock_state},
  {"static", TokenType::Static},
  {"string", TokenType::String},
  {"struct", TokenType::Struct},
  {"switch", TokenType::Switch},
  {"StructuredBuffer", TokenType::StructuredBuffer},
  {"tbuffer", TokenType::Tbuffer},
  {"technique", TokenType::Technique},
  {"technique10", TokenType::Technique10},
  {"technique11", TokenType::Technique11},
  {"texture", TokenType::Texture},
  {"Texture1D", TokenType::Texture1D},
  {"Texture1DArray", TokenType::Texture1DArray},
  {"Texture2D", TokenType::Texture2D},
  {"Texture2DArray", TokenType::Texture2DArray},
  {"Texture2DMS", TokenType::Texture2DMS},
  {"Texture2DMSArray", TokenType::Texture2DMSArray},
  {"Texture3D", TokenType::Texture3D},
  {"TextureCube", TokenType::TextureCube},
  {"TextureCubeArray", TokenType::TextureCubeArray},
  {"true", TokenType::True},
  {"typedef", TokenType::Typedef},
  {"triangle", TokenType::Triangle},
  {"triangleadj", TokenType::Triangleadj},
  {"TriangleStream", TokenType::TriangleStream},
  {"uint", TokenType::Uint},
  {"uniform", TokenType::Uniform},
  {"unorm", TokenType::Unorm},
  {"unsigned", TokenType::Unsigned},
  {"vector", TokenType::Vector},
  {"vertexfragment", TokenType::Vertexfragment},
  {"VertexShader", TokenType::VertexShader},
  {"void", TokenType::Void},
  {"volatile", TokenType::Volatile},
  {"while", TokenType::While},
  {"Expression", TokenType::Expression},
  {"float1", TokenType::Float1},
  {"float1x1", TokenType::Float1x1},
  {"float1x2", TokenType::Float1x2},
  {"float1x3", TokenType::Float1x3},
  {"float1x4", TokenType::Float1x4},
  {"float2", TokenType::Float2},
  {"float2x1", TokenType::Float2x1},
  {"float2x2", TokenType::Float2x2},
  {"float2x3", TokenType::Float2x3},
  {"float2x4", TokenType::Float2x4},
  {"float3", TokenType::Float3},
  {"float3x1", TokenType::Float3x1},
  {"float3x2", TokenType::Float3x2},
  {"float3x3", TokenType::Float3x3},
  {"float3x4", TokenType::Float3x4},
  {"float4", TokenType::Float4},
  {"float4x1", TokenType::Float4x1},
  {"float4x2", TokenType::Float4x2},
  {"float4x3", TokenType::Float4x3},
  {"float4x4", TokenType::Float4x4},
  {"int1", TokenType::Int1},
  {"int1x1", TokenType::Int1x1},
  {"int1x2", TokenType::Int1x2},
  {"int1x3", TokenType::Int1x3},
  {"int1x4", TokenType::Int1x4},
  {"int2", TokenType::Int2},
  {"int2x1", TokenType::Int2x1},
  {"int2x2", TokenType::Int2x2},
  {"int2x3", TokenType::Int2x3},
  {"int2x4", TokenType::Int2x4},
  {"int3", TokenType::Int3},
  {"int3x1", TokenType::Int3x1},
  {"int3x2", TokenType::Int3x2},
  {"int3x3", TokenType::Int3x3},
  {"int3x4", TokenType::Int3x4},
  {"int4", TokenType::Int4},
  {"int4x1", TokenType::Int4x1},
  {"int4x2", TokenType::Int4x2},
  {"int4x3", TokenType::Int4x3},
  {"int4x4", TokenType::Int4x4},
  {"uint1", TokenType::Uint1},
  {"uint1x1", TokenType::Uint1x1},
  {"uint1x2", TokenType::Uint1x2},
  {"uint1x3", TokenType::Uint1x3},
  {"uint1x4", TokenType::Uint1x4},
  {"uint2", TokenType::Uint2},
  {"uint2x1", TokenType::Uint2x1},
  {"uint2x2", TokenType::Uint2x2},
  {"uint2x3", TokenType::Uint2x3},
  {"uint2x4", TokenType::Uint2x4},
  {"uint3", TokenType::Uint3},
  {"uint3x1", TokenType::Uint3x1},
  {"uint3x2", TokenType::Uint3x2},
  {"uint3x3", TokenType::Uint3x3},
  {"uint3x4", TokenType::Uint3x4},
  {"uint4", TokenType::Uint4},
  {"uint4x1", TokenType::Uint4x1},
  {"uint4x2", TokenType::Uint4x2},
  {"uint4x3", TokenType::Uint4x3},
  {"uint4x4", TokenType::Uint4x4},
  {"bool1", TokenType::Bool1},
  {"bool1x1", TokenType::Bool1x1},
  {"bool1x2", TokenType::Bool1x2},
  {"bool1x3", TokenType::Bool1x3},
  {"bool1x4", TokenType::Bool1x4},
  {"bool2", TokenType::Bool2},
  {"bool2x1", TokenType::Bool2x1},
  {"bool2x2", TokenType::Bool2x2},
  {"bool2x3", TokenType::Bool2x3},
  {"bool2x4", TokenType::Bool2x4},
  {"bool3", TokenType::Bool3},
  {"bool3x1", TokenType::Bool3x1},
  {"bool3x2", TokenType::Bool3x2},
  {"bool3x3", TokenType::Bool3x3},
  {"bool3x4", TokenType::Bool3x4},
  {"bool4", TokenType::Bool4},
  {"bool4x1", TokenType::Bool4x1},
  {"bool4x2", TokenType::Bool4x2},
  {"bool4x3", TokenType::Bool4x3},
  {"bool4x4", TokenType::Bool4x4},
  {"min10float1", TokenType::Min10float1},
  {"min10float1x1", TokenType::Min10float1x1},
  {"min10float1x2", TokenType::Min10float1x2},
  {"min10float1x3", TokenType::Min10float1x3},
  {"min10float1x4", TokenType::Min10float1x4},
  {"min10float2", TokenType::Min10float2},
  {"min10float2x1", TokenType::Min10float2x1},
  {"min10float2x2", TokenType::Min10float2x2},
  {"min10float2x3", TokenType::Min10float2x3},
  {"min10float2x4", TokenType::Min10float2x4},
  {"min10float3", TokenType::Min10float3},
  {"min10float3x1", TokenType::Min10float3x1},
  {"min10float3x2", TokenType::Min10float3x2},
  {"min10float3x3", TokenType::Min10float3x3},
  {"min10float3x4", TokenType::Min10float3x4},
  {"min10float4", TokenType::Min10float4},
  {"min10float4x1", TokenType::Min10float4x1},
  {"min10float4x2", TokenType::Min10float4x2},
  {"min10float4x3", TokenType::Min10float4x3},
  {"min10float4x4", TokenType::Min10float4x4},
  {"min16float1", TokenType::Min16float1},
  {"min16float1x1", TokenType::Min16float1x1},
  {"min16float1x2", TokenType::Min16float1x2},
  {"min16float1x3", TokenType::Min16float1x3},
  {"min16float1x4", TokenType::Min16float1x4},
  {"min16float2", TokenType::Min16float2},
  {"min16float2x1", TokenType::Min16float2x1},
  {"min16float2x2", TokenType::Min16float2x2},
  {"min16float2x3", TokenType::Min16float2x3},
  {"min16float2x4", TokenType::Min16float2x4},
  {"min16float3", TokenType::Min16float3},
  {"min16float3x1", TokenType::Min16float3x1},
  {"min16float3x2", TokenType::Min16float3x2},
  {"min16float3x3", TokenType::Min16float3x3},
  {"min16float3x4", TokenType::Min16float3x4},
  {"min16float4", TokenType::Min16float4},
  {"min16float4x1", TokenType::Min16float4x1},
  {"min16float4x2", TokenType::Min16float4x2},
  {"min16float4x3", TokenType::Min16float4x3},
  {"min16float4x4", TokenType::Min16float4x4},
  {"min12int1", TokenType::Min12int1},
  {"min12int1x1", TokenType::Min12int1x1},
  {"min12int1x2", TokenType::Min12int1x2},
  {"min12int1x3", TokenType::Min12int1x3},
  {"min12int1x4", TokenType::Min12int1x4},
  {"min12int2", TokenType::Min12int2},
  {"min12int2x1", TokenType::Min12int2x1},
  {"min12int2x2", TokenType::Min12int2x2},
  {"min12int2x3", TokenType::Min12int2x3},
  {"min12int2x4", TokenType::Min12int2x4},
  {"min12int3", TokenType::Min12int3},
  {"min12int3x1", TokenType::Min12int3x1},
  {"min12int3x2", TokenType::Min12int3x2},
  {"min12int3x3", TokenType::Min12int3x3},
  {"min12int3x4", TokenType::Min12int3x4},
  {"min12int4", TokenType::Min12int4},
  {"min12int4x1", TokenType::Min12int4x1},
  {"min12int4x2", TokenType::Min12int4x2},
  {"min12int4x3", TokenType::Min12int4x3},
  {"min12int4x4", TokenType::Min12int4x4},
  {"min16int1", TokenType::Min16int1},
  {"min16int1x1", TokenType::Min16int1x1},
  {"min16int1x2", TokenType::Min16int1x2},
  {"min16int1x3", TokenType::Min16int1x3},
  {"min16int1x4", TokenType::Min16int1x4},
  {"min16int2", TokenType::Min16int2},
  {"min16int2x1", TokenType::Min16int2x1},
  {"min16int2x2", TokenType::Min16int2x2},
  {"min16int2x3", TokenType::Min16int2x3},
  {"min16int2x4", TokenType::Min16int2x4},
  {"min16int3", TokenType::Min16int3},
  {"min16int3x1", TokenType::Min16int3x1},
  {"min16int3x2", TokenType::Min16int3x2},
  {"min16int3x3", TokenType::Min16int3x3},
  {"min16int3x4", TokenType::Min16int3x4},
  {"min16int4", TokenType::Min16int4},
  {"min16int4x1", TokenType::Min16int4x1},
  {"min16int4x2", TokenType::Min16int4x2},
  {"min16int4x3", TokenType::Min16int4x3},
  {"min16int4x4", TokenType::Min16int4x4},
  {"min16uint1", TokenType::Min16uint1},
  {"min16uint1x1", TokenType::Min16uint1x1},
  {"min16uint1x2", TokenType::Min16uint1x2},
  {"min16uint1x3", TokenType::Min16uint1x3},
  {"min16uint1x4", TokenType::Min16uint1x4},
  {"min16uint2", TokenType::Min16uint2},
  {"min16uint2x1", TokenType::Min16uint2x1},
  {"min16uint2x2", TokenType::Min16uint2x2},
  {"min16uint2x3", TokenType::Min16uint2x3},
  {"min16uint2x4", TokenType::Min16uint2x4},
  {"min16uint3", TokenType::Min16uint3},
  {"min16uint3x1", TokenType::Min16uint3x1},
  {"min16uint3x2", TokenType::Min16uint3x2},
  {"min16uint3x3", TokenType::Min16uint3x3},
  {"min16uint3x4", TokenType::Min16uint3x4},
  {"min16uint4", TokenType::Min16uint4},
  {"min16uint4x1", TokenType::Min16uint4x1},
  {"min16uint4x2", TokenType::Min16uint4x2},
  {"min16uint4x3", TokenType::Min16uint4x3},
  {"min16uint4x4", TokenType::Min16uint4x4},
}; // tokenDefs

const std::string& tokenTypeToString(TokenType t) {
  auto ti = _tokenTypeToString.find(t);
  if (ti == _tokenTypeToString.end()) {
    static const std::string undefined{"Undefined"};
    return undefined;
  }
  return (*ti).second;
}

TokenType findTokenType(const std::string_view& lexeme) {
  // A possible optimization would be to use something like gperf to generate a
  // perfect hash function for the tokenDefs map. I looked at other alternatives to std::map
  // but they all seemed to be slower than std::map for this use case.
  auto ti = tokenDefs.find(lexeme);
  if (ti != tokenDefs.end()) {
    return (*ti).second;
  }

  return matchLiteral(lexeme);
}

} // namespace hlsl
