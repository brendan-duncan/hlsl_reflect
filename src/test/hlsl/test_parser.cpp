#include "../../lib/hlsl/parser.h"
#include "../test.h"

namespace hlsl {

void testParser(Test& test) {
  test.test("Parser empty", [](Test& test) {
    Parser parser("");
    Ast* ast = parser.parse();
    test.isNotNull(ast);
    delete ast;
  });

  test.test("Parser struct", [](Test& test) {
    Parser parser("struct foo { };");
    Ast* ast = parser.parse();
    test.isNotNull(ast);
    delete ast;
  });

  test.test("Parser struct { fields }", [](Test& test) {
    Parser parser(R"(struct foo {
      int a;
      centroid float b;
    };)");
    Ast* ast = parser.parse();
    test.isNotNull(ast);
    delete ast;
  });

  test.test("Parser attributes", [](Test& test) {
    Parser parser(R"(
    [Foo]
    struct foo {
      int a;
      centroid float b;
    };)");
    Ast* ast = parser.parse();
    test.isNotNull(ast);
    delete ast;
  });

  test.test("Parser attributes2", [](Test& test) {
    Parser parser(R"(
    [Foo][Bar]
    struct foo {
      int a;
      centroid float b;
    };)");
    Ast* ast = parser.parse();
    test.isNotNull(ast);
    delete ast;
  });

  test.test("Parser attributes3", [](Test& test) {
    Parser parser(R"(
    [Foo,Bar][FooBar()]
    struct foo {
      int a;
      centroid float b;
    };)");
    Ast* ast = parser.parse();
    test.isNotNull(ast);
    delete ast;
  });

  test.test("const", [](Test& test) {
    Parser parser(R"(static const int _USE_RGBM = 0;)");
    Ast* ast = parser.parse();
    test.isNotNull(ast);
    delete ast;
  });

  test.test("function", [](Test& test) {
    Parser parser(R"(
float Hash(uint s) {})");
    Ast* ast = parser.parse();
    test.isNotNull(ast);
    delete ast;
  });
}

} // namespace hlsl
