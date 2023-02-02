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

  PrintVisitor visitor;
  visitor.visitRoot(ast->root());

  delete ast;
});
