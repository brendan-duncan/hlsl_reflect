#pragma once

#include "../../lib/reader/hlsl/parser.h"
#include "../../lib/visitor/glsl_generator.h"
#include "../../lib/visitor/prune_tree.h"
#include "../test.h"

using namespace reader::hlsl;
using namespace visitor;

namespace prune_tree_tests {

inline void printAst(const std::string_view& source, ast::Ast* ast) {
  if (ast != nullptr) {
    GlslGenerator generator;
    generator.visitRoot(ast->root());
    std::cout << std::flush;
  }
}

static Test test_prune_tree("Test Prune Tree", []() {
  Parser parser(R"(
    float x = 0, y = 1, z = 2;
    int foo() {
      x = y = z = 2;
    }

    float add(float a, float b) {
      return a + b;
    }

    void main() {
      float res = add(x, z);
    }
  )");

  ast::Ast* ast = parser.parse();
  TEST_NOT_NULL(ast);

  VisibilityVisitor reset(false);
  reset.visitRoot(ast->root());
  TEST_FALSE(ast->findFunction("foo")->visible);
  TEST_FALSE(ast->findFunction("add")->visible);
  TEST_FALSE(ast->findGlobalVariable("x")->visible);
  TEST_FALSE(ast->findGlobalVariable("y")->visible);
  TEST_FALSE(ast->findGlobalVariable("z")->visible);

  PruneTree prune(ast);
  prune.prune("main");
  TEST_TRUE(ast->findFunction("add")->visible);
  TEST_FALSE(ast->findFunction("foo")->visible);
  TEST_TRUE(ast->findGlobalVariable("x")->visible);
  TEST_FALSE(ast->findGlobalVariable("y")->visible);
  TEST_TRUE(ast->findGlobalVariable("z")->visible);

  printAst(parser.source(), ast);

  delete ast;
});
  
} // namespace prune_tree_tests
