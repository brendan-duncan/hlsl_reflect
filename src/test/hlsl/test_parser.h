#pragma once

#include "../../lib/hlsl/parser.h"
#include "../../lib/hlsl/print_visitor.h"
#include "../test.h"

using namespace hlsl;

inline void printAst(const std::string_view& source, Ast* ast) {
  if (ast != nullptr) {
    PrintVisitor visitor;
    visitor.visitRoot(ast->root());
    std::cout << std::flush;
  }
}

static Test test_empty("Parser empty", []() {
  Parser parser("");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  delete ast;
});

static Test test_const("Parser const", []() {
  Parser parser(R"(static const int _USE_RGBM = 0;)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_const_2("Parser const 2", []() {
  Parser parser(R"(const float3 magic = float3(0.0f, 0.0, 0);)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_const_3("Parser const 3", []() {
  Parser parser(R"(const float y = x * 2654435769u;)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_postfix_increment("Parser postfix increment", []() {
  Parser parser(R"(float y = x++;)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_prefix_increment("Parser prefix increment", []() {
  Parser parser(R"(float y = ++x;)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_member_access("Parser member access", []() {
  Parser parser(R"(float y = x.y;)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_ternary_expr("Parser ternary expr", []() {
  Parser parser(R"(float y = x < 0.0 ? 1 + x : x;)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_ternary_expr_2("Parser ternary expr 2", []() {
  Parser parser(R"(float y = (x < 0.0) ? 1 + x : x;)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_ternary_expr_3("Parser ternary expr 3", []() {
  Parser parser(R"(float foo() { f = (x < 0.0) ? 1 + x : x; })");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_struct("Parser struct", []() {
  Parser parser("struct foo { };");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_Parser_struct2("Parser struct2", []() {
  Parser parser(R"(struct foo {
    int a;
    centroid float b;
  };)");
  Ast* ast = parser.parse();
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
  Ast* ast = parser.parse();
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
  Ast* ast = parser.parse();
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
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_function("Parser function", []() {
  Parser parser(R"(
float Hash(uint s) {})");
  Ast* ast = parser.parse();
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
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_function_3("Parser function 3", []() {
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
  Ast* ast = parser.parse();
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
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_function_5("Parser inout", []() {
  Parser parser(R"(uint XorShift(inout uint rngState) { })");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_shader("Parser Shader", []() {
  Parser parser(R"(
void LODDitheringTransition(uint3 fadeMaskSeed, float ditherFactor) {
  clip(ditherFactor - p);
})");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_shader_2("Parser block statement", []() {
  Parser parser(R"(
void foo() { {} })");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_member_in_expression("Parser member in expression", []() {
    Parser parser(R"(
float CubeMapFaceID(float3 dir) {
  if (abs(dir.z) >= abs(dir.x) && abs(dir.z) >= abs(dir.y)) {}
})");
  Ast* ast = parser.parse();
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
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_return_ternary("Parser return ternary", []() {
  Parser parser(R"(
float SanitizeFinite(float x) {
  return IsFinite(x) ? x : 0;
})");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_nested_calls("Parser nested calls", []() {
  Parser parser(R"(
  float f = pow(max(abs(base), half(4.8828125e-4)), power);
)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_half("Parser half", []() {
  Parser parser(R"(
half SafePositivePow_half(half base, half power) {
  return pow(max(abs(base), half(4.8828125e-4)), power);
})");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_bool("Parser bool", []() {
  Parser parser(R"(bool b = true;)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
});

static Test test_param_init("Parser param init", []() {
  Parser parser(R"(void foo(float a = 1.0) { })");
  Ast* ast = parser.parse();
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
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  printAst(parser.source(), ast);
  delete ast;
  free(hlsl);
});
