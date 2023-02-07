#include <iostream>

#include "visitor.h"

namespace hlsl {

/// A visitor that prints the AST to stdout.
/// This will print it out in a format that is similar to Swift, to demonstrate translating
/// to a different syntax style.
class PrintVisitor : public Visitor {
public:
  std::ostream &_out;

  PrintVisitor(std::ostream &out = std::cout)
    : _out(out) {}

  void visitBlock(AstBlock* node) override {
    indent();
    _out << "{" << std::endl;

    _indent++;
    Visitor::visitBlock(node);
    _indent--;

    indent();
    _out << "}" << std::endl;
  }

  void visitBufferStmt(AstBufferStmt* node) override {
    indent();
    if (node->bufferType == BufferType::Cbuffer) {
      _out << "cbuffer ";
    } else {
      _out << "tbuffer ";
    }
    _out << node->name << " {" << std::endl;
    _indent++;
    Visitor::visitBufferStmt(node);
    _indent--;
    indent();
    _out << "}" << std::endl;
  }

  void visitBufferField(AstField* node) {
    indent();
    _out << " " << node->name << ": ";
    visitType(node->type);
    _out << ";" << std::endl;
  }

  void visitStructStmt(AstStructStmt* node) override {
    indent();
    _out << "struct " << node->name << " {" << std::endl;
    _indent++;
    Visitor::visitStructStmt(node);
    _indent--;
    indent();
    _out << "}" << std::endl;
  }

  void visitStructField(AstField* node) override {
    indent();
    _out << " " << node->name << ": ";
    visitType(node->type);
    _out << ";" << std::endl;
  }

  void visitStatement(AstStatement* node) override {
    if (node->nodeType != AstNodeType::Block) {
      indent();
    }
    Visitor::visitStatement(node);
    if (node->nodeType != AstNodeType::Block && node->nodeType != AstNodeType::IfStmt) {
      _out << ";" << std::endl;
    }
  }

  void visitSwitchStmt(AstSwitchStmt* node) override {
    _out << "switch (";
    visitExpression(node->condition);
    _out << ") {" << std::endl;
    _indent++;
    AstSwitchCase* switchCase = node->cases;
    while (switchCase != nullptr) {
      visitSwitchCase(switchCase);
      switchCase = switchCase->next;
    }
    _indent--;
    indent();
    _out << "}" << std::endl;
  }

  void visitSwitchCase(AstSwitchCase* node) override {
    indent();
    if (node->isDefault) {
      _out << "default:" << std::endl;
    } else {
      _out << "case ";
      visitExpression(node->condition);
      _out << ":" << std::endl;
    }
    _indent++;
    visitStatements(node->body);
    _indent--;
  }

  void visitBreakStmt(AstBreakStmt* node) override {
    _out << "break";
  }

  void visitFunctionStmt(AstFunctionStmt* node) override {
    _out << "fn " << node->name << "(";
    visitParameters(node->parameters);
    _out << ") -> ";
    visitType(node->returnType);
    _out << std::endl;
    visitBlock(node->body);
    _out << std::endl;
  }

  void visitVariableStmt(AstVariableStmt* node) override {
    if (node->type->flags & TypeFlags::Const) {
      _out << "const ";
    } else {
      _out << "var ";
    }
    _out << node->name << ": ";
    visitType(node->type);

    AstLiteralExpr* arraySize = node->arraySize;
    while (arraySize != nullptr) {
      _out << "[" << arraySize->value << "]";
      arraySize = (AstLiteralExpr*)arraySize->next;
    }

    if (node->initializer != nullptr) {
      _out << " = ";
      visitExpression(node->initializer);
    }
    _out << std::flush;
  }

  void visitParameter(AstParameter* node) override {
    _out << node->name << ": ";
    visitType(node->type);
    if (node->initializer != nullptr) {
      _out << " = ";
      visitExpression(node->initializer);
    }
  }

  void visitParameters(AstParameter* node) override {
    while (node != nullptr) {
      visitParameter(node);
      if (node->next != nullptr) {
        _out << ", ";
      }
      node = (AstParameter*)node->next;
    }
  }

  void visitType(AstType* node) override {
    _out << baseTypeToString(node->baseType);
  }

  void visitIfStmt(AstIfStmt* node) override {
    _out << "if (";
    visitExpression(node->condition);
    _out << ")" << std::endl;
    visitStatement(node->body);
    if (node->elseBody != nullptr) {
      indent();
      _out << "else" << std::endl;
      visitStatement(node->elseBody);
    }
  }

  void visitAssignmentStmt(AstAssignmentStmt* node) override {
    visitExpression(node->variable);
    _out << " " << operatorToString(node->op) << " ";
    visitExpression(node->value);
  }

  void visitAssignmentExpr(AstAssignmentExpr* node) override {
    visitExpression(node->variable);
    _out << " " << operatorToString(node->op) << " ";
    visitExpression(node->value);
  }

  void visitBinaryExpr(AstBinaryExpr *node) override {
    visitExpression(node->left);
    _out << " " << operatorToString(node->op) << " ";
    visitExpression(node->right);
  }

  void visitVariableExpr(AstVariableExpr* node) override {
    _out << node->name;
  }

  void visitLiteralExpr(AstLiteralExpr* node) override {
    _out << node->value;
  }

  void visitReturn(AstReturnStmt* node) override {
    _out << "return ";
    visitExpression(node->value);
  }

  void visitCallStmt(AstCallStmt* node) override {
    _out << node->name << "(";
    visitArguments(node->arguments);
    _out << ");";
  }

  void visitCallExpr(AstCallExpr* node) override {
    _out << node->name << "(";
    visitArguments(node->arguments);
    _out << ")";
  }

  void visitCastExpr(AstCastExpr* node) override {
    visitType(node->type);
    _out << "(";
    visitArguments(node->value);
    _out << ")";
  }

  void visitArrayInitializerExpr(AstArrayInitializerExpr* node) {
    _out << "{";
    for (AstExpression* expr = node->elements; expr != nullptr; expr = expr->next) {
      visitExpression(expr);
      if (expr->next != nullptr) {
        _out << ", ";
      }
    }
    _out << "}";
  }

  void visitStructInitializerExpr(AstStructInitializerExpr* node) {
    _out << "{";
    for (AstExpression* expr = node->fields; expr != nullptr; expr = expr->next) {
      visitExpression(expr);
      if (expr->next != nullptr) {
        _out << ", ";
      }
    }
    _out << "}";
  }

  void visitArguments(AstExpression* args) override {
    while (args != nullptr) {
      visitExpression(args);
      if (args->next != nullptr) {
        _out << ", ";
      }
      args = args->next;
    }
  }

  void visitIncrementExpr(AstIncrementExpr* node) override {
    visitExpression(node->variable);
    _out << operatorToString(node->op);
  }

  void visitPrefixExpr(AstPrefixExpr* node) override {
    _out << operatorToString(node->op);
    visitExpression(node->expression);
  }

  void visitMemberExpr(AstMemberExpr* node) override {
    visitExpression(node->object);
    _out << ".";
    visitExpression(node->member);
  }

  void visitTernaryExpr(AstTernaryExpr* node) override {
    visitExpression(node->condition);
    _out << " ? ";
    visitExpression(node->trueExpr);
    _out << " : ";
    visitExpression(node->falseExpr);
  }

  void visitArrayExpr(AstArrayExpr* node) {
    visitExpression(node->array);
    _out << "[";
    visitExpression(node->index);
    _out << "]";
  }

private:
  int _indent = 0;

  void indent() {
    for (int i = 0; i < _indent; ++i) {
      _out << "  ";
    }
  }
};

} // namespace hlsl
