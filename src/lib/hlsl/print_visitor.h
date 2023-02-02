#include <iostream>

#include "visitor.h"

namespace hlsl {

/// A visitor that prints the AST to stdout.
class PrintVisitor : public Visitor {
public:
  void visitBlock(AstBlock* node) override {
    indent();
    std::cout << "{" << std::endl;

    _indent++;
    Visitor::visitBlock(node);
    _indent--;

    indent();
    std::cout << "}" << std::endl;
  }

  void visitFunction(AstFunction* node) override {
    indent();
    std::cout << "fn " << node->name << "(";
    visitParameters(node->parameters);
    std::cout << ") -> ";
    visitType(node->returnType);
    visitBlock(node->body);
    std::cout << std::endl;
  }

  void visitParameter(AstParameter* node) override {
    std::cout << node->name << ": ";
    visitType(node->type);
    if (node->initializer != nullptr) {
      std::cout << " = ";
      visitExpression(node->initializer);
    }
  }

  void visitParameters(AstParameter* node) override {
    while (node != nullptr) {
      visitParameter(node);
      if (node->next != nullptr) {
        std::cout << ", ";
      }
      node = (AstParameter*)node->next;
    }
  }

  void visitType(AstType* node) override {
    std::cout << node->name << ": " << baseTypeToString(node->baseType);
  }

private:
  int _indent = 0;

  void indent() {
    for (int i = 0; i < _indent; ++i) {
      std::cout << "  ";
    }
  }
};

} // namespace hlsl
