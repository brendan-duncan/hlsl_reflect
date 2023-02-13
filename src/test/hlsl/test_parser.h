#pragma once

#include "../../lib/reader/hlsl/parser.h"
#include "../../lib/visitor/print_visitor.h"
#include "../test.h"

using namespace hlsl;
using namespace visitor;

namespace parser_tests {

inline void printAst(const std::string_view& source, ast::Ast* ast) {
  if (ast != nullptr) {
    PrintVisitor visitor;
    visitor.visitRoot(ast->root());
    std::cout << std::flush;
  }
}

static Test test_empty("Parser empty", []() {
  Parser parser("");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  delete ast;
});

static Test test_static_const_int("Parser const int", []() {
  Parser parser(R"(static const int _USE_RGBM = 0;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_float3("Parser float3", []() {
  Parser parser(R"(const float3 magic = float3(0.0f, 0.0, 0);)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_array_init("Parser array init", []() {
  Parser parser(R"(const float3 magic = {0.0f, 0.0, 0};)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_array_init_trailing_comma("Parser array init trailing comma", []() {
  Parser parser(R"(const float3 magic = {0.0f, 0.0, 0,};)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_array_multi_array("Parser multi array", []() {
  Parser parser(R"(static const float foo[2][2] = {{1,2},{3,4}};)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_const_init_expression("Parser const init expression", []() {
  Parser parser(R"(const float y = (x * 2654435769u);)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_cast_1("Parser cast 1", []() {
  Parser parser(R"(const float y = float(0);)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_cast_2("Parser cast 2", []() {
  Parser parser(R"(const float y = (float)0;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_struct_cast("Parser struct cast", []() {
  Parser parser(R"(struct foo { }; const foo y = (foo)0;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_multi_variable("Parser multi variable", []() {
  Parser parser(R"(float x = 0, y = 1;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_multi_variable_assignment("Parser multi variable assignment", []() {
  Parser parser(R"(
    float x = 0, y = 1;
    int foo() {
      a = b = c = d = 0;
    }
  )");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_multi_variable_array("Parser multi variable array", []() {
  Parser parser(R"(float2 weights[2], offsets[2];)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_empty_statement("Parser empty statement", []() {
  Parser parser(R"(void foo() {
    float foo = 1;;
    #line 55
  })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_call_method("Parser call method", []() {
  Parser parser(R"(#line 930
void foo() {
  foo.bar(a, b);
})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_multiply_assign("Parser call method 2", []() {
  Parser parser(R"(void foo() { n *= 1e-6; })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_postfix_increment("Parser postfix increment", []() {
  Parser parser(R"(float y = x++;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_prefix_increment("Parser prefix increment", []() {
  Parser parser(R"(float y = ++x;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_member_access("Parser member access", []() {
  Parser parser(R"(float y = x.y;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_ternary_expr("Parser ternary expr", []() {
  Parser parser(R"(float y = x < 0.0 ? 1 + x : x;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_ternary_expr_2("Parser ternary expr 2", []() {
  Parser parser(R"(float y = (x < 0.0) ? 1 + x : x;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_ternary_expr_3("Parser ternary expr 3", []() {
  Parser parser(R"(float foo() { f = (x < 0.0) ? 1 + x : x; })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_struct("Parser struct", []() {
  Parser parser("struct foo { };");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_struct_var("Parser struct var", []() {
  Parser parser("struct { } foo;");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_cbuffer_struct_var("Parser cbuffer struct var", []() {
  Parser parser("cbuffer foo { struct { } bar; };");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_struct_fields("Parser struct fields", []() {
  Parser parser(R"(struct foo {
    int a;
    centroid float b;
    float c[10];
  };)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_struct_field_semantic("Parser struct field semantic", []() {
  Parser parser(R"(struct foo { uint vertexID : SV_VertexID; };)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_struct_member_multi_decl("Parser struct field multi-declration", []() {
  Parser parser(R"(struct foo { int a, b, c; };)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_struct_init("Parser struct init", []() {
  Parser parser(R"(struct foo { float a[2]; float b; };
  foo x = { {1, 2}, 3 };)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_attributes("Parser attributes", []() {
  Parser parser(R"(
  [Foo]
  struct foo {
    int a;
    centroid float b;
  };)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_attributes2("Parser attributes2", []() {
  Parser parser(R"(
  [Foo][Bar]
  struct foo {
    int a;
    centroid float b;
  };)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_attributes3("Parser attributes3", []() {
  Parser parser(R"(
  [Foo,Bar][FooBar()]
  struct foo {
    int a;
    centroid float b;
  };)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_function("Parser function", []() {
  Parser parser(R"(
float Hash(uint s) {})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_function_2("Parser function 2", []() {
  Parser parser(R"(
float Hash(uint s) {
s = s ^ 2747636419u;
s = s * 2654435769u;
s = s ^ (s >> 16);
s = s * 2654435769u;
s = s ^ (s >> 16);
s = s * 2654435769u;
return float(s) * rcp(4294967296.0);
})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_function_3("Parser multiple functions", []() {
  Parser parser(R"(
#line 5 "D:/Random.hlsl"
float Hash(uint s)
{
s = s ^ 2747636419u;
s = s * 2654435769u;
s = s ^ (s >> 16);
s = s * 2654435769u;
s = s ^ (s >> 16);
s = s * 2654435769u;
return float(s) * rcp(4294967296.0);
}
#line 19
uint JenkinsHash(uint x) {
x += (x << 10u);
x ^= (x >> 6u);
x += (x << 3u);
x ^= (x >> 11u);
x += (x << 15u);
return x ;
})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_function_4("Parser function 4", []() {
  Parser parser(R"(
float ConstructFloat(int m) {
const int ieeeMantissa = 0x007FFFFF;
const int ieeeOne = 0x3F800000;

m &= ieeeMantissa;
m |= ieeeOne;

float f = asfloat(m);
return f - 1;
})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_user_type_return("Parser struct return", []() {
  Parser parser(R"(
struct MyStruct { float3 f3; };
MyStruct Foo() {})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_assignment_return("Parser assignment return", []() {
  Parser parser(R"(float foo() { float a; return a *= 1 + 2; })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_function_semantic("Parser function semantic", []() {
  Parser parser(R"(float4 foo(): SV_Target {})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_function_5("Parser inout", []() {
  Parser parser(R"(uint XorShift(inout uint rngState) { })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_const_array_size("Parser const array size", []() {
  Parser parser(R"(const int stepCount = 2; const int gWeights[stepCount] = {0, 1};)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_function_array_params("Parser function array parameters", []() {
  Parser parser(R"(void foo(out float2 bar[2]) { bar[0] = 42; })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_shader("Parser Shader", []() {
  Parser parser(R"(
void LODDitheringTransition(uint3 fadeMaskSeed, float ditherFactor) {
  clip(ditherFactor - p);
})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_shader_2("Parser block statement", []() {
  Parser parser(R"(
void foo() { {} })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_member_in_expression("Parser member in expression", []() {
    Parser parser(R"(
float CubeMapFaceID(float3 dir) {
  if (abs(dir.z) >= abs(dir.x) && abs(dir.z) >= abs(dir.y)) {}
})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_shader_3("Parser if else", []() {
  Parser parser(R"(
void foo() {
  if (a) {
  } else if (b) {
  }
  return faceID;
})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_return_ternary("Parser return ternary", []() {
  Parser parser(R"(
float SanitizeFinite(float x) {
  return IsFinite(x) ? x : 0;
})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_nested_calls("Parser nested calls", []() {
  Parser parser(R"(
  float f = pow(max(abs(base), half(4.8828125e-4)), power);
)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_half("Parser half", []() {
  Parser parser(R"(
half SafePositivePow_half(half base, half power) {
  return pow(max(abs(base), half(4.8828125e-4)), power);
})");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_bool("Parser bool", []() {
  Parser parser(R"(bool b = true;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_param_init("Parser param init", []() {
  Parser parser(R"(void foo(float a = 1.0) { })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_array_index("Parser array index", []() {
  Parser parser(R"(float m12 = m[1][2];)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_for_loop("Parser for loop", []() {
  Parser parser(R"(void foo() { for (uint b = 1U << firstbithigh(n - 1); b != 0; b >>= 1) {} })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_for_loop_2("Parser for loop 2", []() {
  Parser parser(R"(void foo() { for (int i = 0, j = 0; i < 5; ++i, j++) {} })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_rw_structure_buffer("Parser rw structure buffer", []() {
  Parser parser(R"(RWStructuredBuffer<float> g_fogDensityBuffer;
  sampler<float> g_fogDensitySampler;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_switch_case_block("Parser switch", []() {
  Parser parser(R"(
  void foo() { switch (bar) { case 1: sampleCount = 21; break; default: {} } })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_parameter_semantic("Parser parameter semantic", []() {
  Parser parser(R"(void SplatmapFragment(float IN, out half4 outColor : SV_Target0) { })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_parameter_default_value("Parser parameter default value", []() {
  Parser parser(R"(void UnpackNormalAG(float4 packedNormal, float scale = 1.0) { })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_multi_statement("Parser multi statement", []() {
  Parser parser(R"(void foo() { surface.smoothness = saturate(surfaceDescription.Smoothness),
    surface.occlusion = surfaceDescription.Occlusion; })");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_texture2dms("Parser Texture2DMS", []() {
  Parser parser(R"(Texture2DMS<float, 4> _CameraDepthAttachment;)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_array_const_expr_size("Parser Array const expr size", []() {
  Parser parser(R"(float4 _TextureInfo[8 * 2];)");
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parse_urp("Parse urp_bloom", []() {
  FILE* fp = fopen(TEST_DATA_PATH("/hlsl/urp_bloom.hlsl"), "rb");
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char* hlsl = (char*)calloc(1, size + 1);
  fread(hlsl, size, 1, fp);
  hlsl[size] = '\0';
  fclose(fp);

  Parser parser(hlsl);
  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  //printAst(parser.source(), ast);
  delete ast;
  free(hlsl);
});

} // namespace parser_tests
