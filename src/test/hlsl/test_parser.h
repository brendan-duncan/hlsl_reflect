#include "../../lib/hlsl/parser.h"
#include "../../lib/hlsl/print_visitor.h"
#include "../test.h"

using namespace hlsl;

TEST(Parser_empty, []() {
  Parser parser("");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  delete ast;
});

TEST(Parser_struct, []() {
  Parser parser("struct foo { };");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  delete ast;
});

TEST(Parser_struct2, []() {
  Parser parser(R"(struct foo {
    int a;
    centroid float b;
  };)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  delete ast;
});

TEST(Parser_attributes, []() {
  Parser parser(R"(
  [Foo]
  struct foo {
    int a;
    centroid float b;
  };)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  delete ast;
});

TEST(Parser_attributes2, []() {
  Parser parser(R"(
  [Foo][Bar]
  struct foo {
    int a;
    centroid float b;
  };)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  delete ast;
});

TEST(Parser_attributes3, []() {
  Parser parser(R"(
  [Foo,Bar][FooBar()]
  struct foo {
    int a;
    centroid float b;
  };)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  delete ast;
});

TEST(const, []() {
  Parser parser(R"(static const int _USE_RGBM = 0;)");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  delete ast;
});

TEST(function, []() {
  Parser parser(R"(
float Hash(uint s) {})");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);
  delete ast;
});

static Test test_function_2("function_2", []() {
  Parser parser(R"(
float Hash(uint s) {
s = s ^ 2747636419u ;
s = s * 2654435769u ;
s = s ^ ( s >> 16 ) ;
s = s * 2654435769u ;
s = s ^ ( s >> 16 ) ;
s = s * 2654435769u ;
return float(s) * rcp(4294967296.0);
})");
  Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);

  if (ast != nullptr) {
    PrintVisitor visitor;
    visitor.visitRoot(ast->root());
  }

  delete ast;
});

static Test test_function_3("function_3", []() {
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

  if (ast != nullptr) {
    PrintVisitor visitor;
    visitor.visitRoot(ast->root());
  }

  delete ast;
});

static Test test_function_4("function_4", []() {
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

  if (ast != nullptr) {
    PrintVisitor visitor;
    visitor.visitRoot(ast->root());
  }

  delete ast;
});

static Test test_Parse_Shader("Parse_Shader", []() {
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

  if (ast != nullptr) {
    PrintVisitor visitor;
    visitor.visitRoot(ast->root());
  }

  delete ast;
  free(hlsl);
});
