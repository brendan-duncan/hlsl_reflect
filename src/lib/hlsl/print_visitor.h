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

  void visitStructStmt(AstStructStmt* node) override {
    indent();
    std::cout << "struct " << node->name << " {" << std::endl;
    _indent++;
    Visitor::visitStructStmt(node);
    _indent--;
    indent();
    std::cout << "}" << std::endl;
  }

  void visitStructField(AstStructField* node) override {
    indent();
    std::cout << " " << node->name << ": ";
    visitType(node->type);
    std::cout << ";" << std::endl;
  }

  void visitStatement(AstStatement* node) override {
    if (node->nodeType != AstNodeType::Block) {
      indent();
    }
    Visitor::visitStatement(node);
    if (node->nodeType != AstNodeType::Block && node->nodeType != AstNodeType::IfStmt) {
      std::cout << ";" << std::endl;
    }
  }

  void visitFunctionStmt(AstFunctionStmt* node) override {
    std::cout << "fn " << node->name << "(";
    visitParameters(node->parameters);
    std::cout << ") -> ";
    visitType(node->returnType);
    std::cout << std::endl;
    visitBlock(node->body);
    std::cout << std::endl;
  }

  void visitVariableStmt(AstVariableStmt* node) override {
    if (node->type->flags & TypeFlags::Const) {
      std::cout << "const ";
    } else {
      std::cout << "var ";
    }
    std::cout << node->name << ": ";
    visitType(node->type);
    if (node->initializer != nullptr) {
      std::cout << " = ";
      visitExpression(node->initializer);
    }
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

  void visitIfStmt(AstIfStmt* node) override {
    std::cout << "if (";
    visitExpression(node->condition);
    std::cout << ")" << std::endl;
    visitStatement(node->body);
    if (node->elseBody != nullptr) {
      indent();
      std::cout << "else" << std::endl;
      visitStatement(node->elseBody);
    }
  }

  void visitAssignmentStmt(AstAssignmentStmt* node) override {
    visitExpression(node->variable);
    std::cout << " " << operatorToString(node->op) << " ";
    visitExpression(node->value);
  }

  void visitAssignmentExpr(AstAssignmentExpr* node) override {
    visitExpression(node->variable);
    std::cout << " " << operatorToString(node->op) << " ";
    visitExpression(node->value);
  }

  void visitBinaryExpr(AstBinaryExpr *node) override {
    visitExpression(node->left);
    std::cout << " " << operatorToString(node->op) << " ";
    visitExpression(node->right);
  }

  void visitVariableExpr(AstVariableExpr* node) override {
    std::cout << node->name;
  }

  void visitLiteralExpr(AstLiteralExpr* node) override {
    std::cout << node->value;
  }

  void visitReturn(AstReturnStmt* node) override {
    std::cout << "return ";
    visitExpression(node->value);
  }

  void visitCallStmt(AstCallStmt* node) override {
    std::cout << node->name << "(";
    visitArguments(node->arguments);
    std::cout << ");";
  }

  void visitCallExpr(AstCallExpr* node) override {
    std::cout << node->name << "(";
    visitArguments(node->arguments);
    std::cout << ")";
  }

  void visitCastExpr(AstCastExpr* node) override {
    visitType(node->type);
    std::cout << "(";
    visitArguments(node->value);
    std::cout << ")";
  }

  void visitArguments(AstExpression* args) override {
    while (args != nullptr) {
      visitExpression(args);
      if (args->next != nullptr) {
        std::cout << ", ";
      }
      args = args->next;
    }
  }

  void visitIncrementExpr(AstIncrementExpr* node) override {
    visitExpression(node->variable);
    std::cout << operatorToString(node->op);
  }

  void visitPrefixExpr(AstPrefixExpr* node) override {
    std::cout << operatorToString(node->op);
    visitExpression(node->expression);
  }

  void visitMemberExpr(AstMemberExpr* node) override {
    visitExpression(node->object);
    std::cout << ".";
    visitExpression(node->member);
  }

  void visitTernaryExpr(AstTernaryExpr* node) override {
    visitExpression(node->condition);
    std::cout << " ? ";
    visitExpression(node->trueExpr);
    std::cout << " : ";
    visitExpression(node->falseExpr);
  }

  void visitArrayExpr(AstArrayExpr* node) {
    visitExpression(node->array);
    std::cout << "[";
    visitExpression(node->index);
    std::cout << "]";
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
