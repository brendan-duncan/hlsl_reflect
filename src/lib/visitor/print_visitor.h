#include <iostream>

#include "visitor.h"

namespace visitor {

/// A visitor that prints the AST to stdout.
/// This will print it out in a format that is similar to Swift, to demonstrate translating
/// to a different syntax style.
class PrintVisitor : public Visitor {
public:
  std::ostream &_out;

  PrintVisitor(std::ostream &out = std::cout)
    : _out(out) {}

  void visitBlock(ast::AstBlock* node) override {
    _out << "{" << std::endl;

    _indent++;
    Visitor::visitBlock(node);
    _indent--;

    indent();
    _out << "}" << std::endl;
  }

  void visitStatements(ast::AstStatement* node) override {
    while (node) {
      indent();
      visitStatement(node);
      node = node->next;
      _out << std::endl;
    }
  }

  void visitBufferStmt(ast::AstBufferStmt* node) override {
    indent();
    if (node->bufferType == ast::BufferType::Cbuffer) {
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

  void visitBufferField(ast::AstField* node) {
    indent();
    _out << " " << node->name << ": ";
    visitType(node->type);
    _out << ";" << std::endl;
  }

  void visitStructStmt(ast::AstStructStmt* node) override {
    indent();
    _out << "struct " << node->name << " {" << std::endl;
    _indent++;
    Visitor::visitStructStmt(node);
    _indent--;
    indent();
    _out << "}" << std::endl;
  }

  void visitStructField(ast::AstField* node) override {
    indent();
    _out << " " << node->name << ": ";
    visitType(node->type);
    _out << ";" << std::endl;
  }

  void visitStatement(ast::AstStatement* node) override {
    Visitor::visitStatement(node);
    if (node->nodeType != ast::AstNodeType::Block &&
        node->nodeType != ast::AstNodeType::IfStmt &&
        node->nodeType != ast::AstNodeType::ForStmt &&
        node->nodeType != ast::AstNodeType::SwitchStmt &&
        node->nodeType != ast::AstNodeType::WhileStmt) {
      _out << ";";
    }
  }

  void visitForStmt(ast::AstForStmt* node) override {
    _out << "for (";
    ast::AstStatement* init = node->initializer;
    while (init != nullptr) {
      visitStatement(init);
      if (init->next != nullptr) {
        _out << ", ";
      }
      init = init->next;
    }
    if (node->initializer == nullptr) {
      _out << "; ";
    } else {
      _out << " ";
    }

    visitExpression(node->condition);
    _out << "; ";
    
    ast::AstStatement* inc = node->increment;
    while (inc != nullptr) {
      visitStatement(inc);
      if (inc->next != nullptr) {
        _out << ", ";
      }
      inc = inc->next;
    }
    _out << ")" << std::endl;

    visitStatements(node->body);
  }

  void visitSwitchStmt(ast::AstSwitchStmt* node) override {
    _out << "switch (";
    visitExpression(node->condition);
    _out << ") {" << std::endl;
    _indent++;
    ast::AstSwitchCase* switchCase = node->cases;
    while (switchCase != nullptr) {
      visitSwitchCase(switchCase);
      switchCase = switchCase->next;
    }
    _indent--;
    indent();
    _out << "}" << std::endl;
  }

  void visitSwitchCase(ast::AstSwitchCase* node) override {
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

  void visitBreakStmt(ast::AstBreakStmt* node) override {
    _out << "break";
  }

  void visitFunctionStmt(ast::AstFunctionStmt* node) override {
    _out << "fn " << node->name << "(";
    visitParameters(node->parameters);
    _out << ") -> ";
    visitType(node->returnType);
    _out << std::endl;
    visitBlock(node->body);
    _out << std::endl;
  }

  void visitVariableStmt(ast::AstVariableStmt* node) override {
    if (node->type->flags & ast::TypeFlags::Const) {
      _out << "const ";
    } else {
      _out << "var ";
    }
    _out << node->name << ": ";
    visitType(node->type);

    ast::AstExpression* arraySize = node->arraySize;
    while (arraySize != nullptr) {
      _out << "[";
      visitExpression(arraySize);
       _out << "]";
      arraySize = (ast::AstLiteralExpr*)arraySize->next;
    }

    if (node->initializer != nullptr) {
      _out << " = ";
      visitExpression(node->initializer);
    }
    _out << std::flush;
  }

  void visitParameter(ast::AstParameter* node) override {
    _out << node->name << ": ";
    visitType(node->type);
    if (node->initializer != nullptr) {
      _out << " = ";
      visitExpression(node->initializer);
    }
  }

  void visitParameters(ast::AstParameter* node) override {
    while (node != nullptr) {
      visitParameter(node);
      if (node->next != nullptr) {
        _out << ", ";
      }
      node = (ast::AstParameter*)node->next;
    }
  }

  void visitType(ast::AstType* node) override {
    _out << baseTypeToString(node->baseType);
  }

  void visitIfStmt(ast::AstIfStmt* node) override {
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

  void visitAssignmentStmt(ast::AstAssignmentStmt* node) override {
    visitExpression(node->variable);
    _out << " " << operatorToString(node->op) << " ";
    visitExpression(node->value);
  }

  void visitAssignmentExpr(ast::AstAssignmentExpr* node) override {
    visitExpression(node->variable);
    _out << " " << operatorToString(node->op) << " ";
    visitExpression(node->value);
  }

  void visitBinaryExpr(ast::AstBinaryExpr *node) override {
    visitExpression(node->left);
    _out << " " << operatorToString(node->op) << " ";
    visitExpression(node->right);
  }

  void visitVariableExpr(ast::AstVariableExpr* node) override {
    _out << node->name;
  }

  void visitLiteralExpr(ast::AstLiteralExpr* node) override {
    _out << node->value;
  }

  void visitReturn(ast::AstReturnStmt* node) override {
    _out << "return ";
    visitExpression(node->value);
  }

  void visitCallStmt(ast::AstCallStmt* node) override {
    _out << node->name << "(";
    visitArguments(node->arguments);
    _out << ");";
  }

  void visitCallExpr(ast::AstCallExpr* node) override {
    _out << node->name << "(";
    visitArguments(node->arguments);
    _out << ")";
  }

  void visitCastExpr(ast::AstCastExpr* node) override {
    visitType(node->type);
    _out << "(";
    visitArguments(node->value);
    _out << ")";
  }

  void visitArrayInitializerExpr(ast::AstArrayInitializerExpr* node) {
    _out << "{";
    for (ast::AstExpression* expr = node->elements; expr != nullptr; expr = expr->next) {
      visitExpression(expr);
      if (expr->next != nullptr) {
        _out << ", ";
      }
    }
    _out << "}";
  }

  void visitStructInitializerExpr(ast::AstStructInitializerExpr* node) {
    _out << "{";
    for (ast::AstExpression* expr = node->fields; expr != nullptr; expr = expr->next) {
      visitExpression(expr);
      if (expr->next != nullptr) {
        _out << ", ";
      }
    }
    _out << "}";
  }

  void visitArguments(ast::AstExpression* args) override {
    while (args != nullptr) {
      visitExpression(args);
      if (args->next != nullptr) {
        _out << ", ";
      }
      args = args->next;
    }
  }

  void visitIncrementExpr(ast::AstIncrementExpr* node) override {
    visitExpression(node->variable);
    _out << operatorToString(node->op);
  }

  void visitPrefixExpr(ast::AstPrefixExpr* node) override {
    _out << operatorToString(node->op);
    visitExpression(node->expression);
  }

  void visitMemberExpr(ast::AstMemberExpr* node) override {
    visitExpression(node->object);
    _out << ".";
    visitExpression(node->member);
  }

  void visitTernaryExpr(ast::AstTernaryExpr* node) override {
    visitExpression(node->condition);
    _out << " ? ";
    visitExpression(node->trueExpr);
    _out << " : ";
    visitExpression(node->falseExpr);
  }

  void visitArrayExpr(ast::AstArrayExpr* node) {
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

} // namespace visitor
