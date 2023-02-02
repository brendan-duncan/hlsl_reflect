#include <iostream>

#include "visitor.h"

namespace hlsl {

/// A visitor that prints the AST to stdout.
/// This will print it out in a format that is similar to Swift, to demonstrate translating
/// to a different syntax style.
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

  void visitStatement(AstStatement* node) override {
    indent();
    Visitor::visitStatement(node);
    std::cout << ";" << std::endl;
  }

  void visitFunction(AstFunction* node) override {
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
    std::cout << baseTypeToString(node->baseType);
  }

  void visitAssignment(AstAssignment* node) override {
    visitExpression(node->variable);
    std::cout << " = ";
    visitExpression(node->value);
  }

  void visitBinaryOperator(AstBinaryOperator *node) {
    visitExpression(node->left);
    std::cout << " " << operatorToString(node->op) << " ";
    visitExpression(node->right);
  }

  void visitVariable(AstVariable* node) override {
    std::cout << node->name;
  }

  void visitVariableExpr(AstVariableExpr* node) override {
    std::cout << node->name;
  }

  void visitLiteralExpr(AstLiteralExpr* node) override {
    std::cout << node->value;
  }

  void visitCallExpr(AstCallExpr* node) {
    std::cout << node->name << "(";
    visitArguments(node->arguments);
    std::cout << ")";
  }

  void visitCastExpr(AstCastExpr* node) {
    visitType(node->type);
    std::cout << "(";
    visitExpression(node->expression);
    std::cout << ")";
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
