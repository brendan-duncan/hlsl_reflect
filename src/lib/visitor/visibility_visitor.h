#include "../ast/ast.h"
#include "visitor.h"

namespace visitor {

class VisibilityVisitor : public Visitor {
public:
  bool visibility;

  VisibilityVisitor(bool visibility)
      : visibility(visibility) {}

  void visitFunctionStmt(ast::FunctionStmt* node) override {
    node->visible = visibility;
    Visitor::visitFunctionStmt(node);
  }

  void visitVariableStmt(ast::VariableStmt* node) override {
    node->visible = visibility;
    ast::Statement* next = node->next;
    while (next) {
      if (next->nodeType == ast::NodeType::VariableStmt) {
        ast::VariableStmt* var = static_cast<ast::VariableStmt*>(next);
        var->visible = visibility;
      }
      next = next->next;
    }
    Visitor::visitVariableStmt(node);
  }

  void visitBufferStmt(ast::BufferStmt* node) override {
    node->visible = visibility;
    Visitor::visitBufferStmt(node);
  }

  void visitStructField(ast::Field* node) override {
    node->visible = visibility;
  }

  void visitParameter(ast::Parameter* node) override {
    node->visible = visibility;
  }
};

class MarkVisibleStatements : public Visitor {
public:
  ast::Ast* ast;

  MarkVisibleStatements(ast::Ast *ast)
    : ast(ast) {
  }

  void visitFunctionStmt(ast::FunctionStmt* node) override {
    node->visible = true;
    Visitor::visitFunctionStmt(node);
  }

  void visitCallStmt(ast::CallStmt* node) override {
    Visitor::visitCallStmt(node);
    ast::FunctionStmt* function = ast->findFunction(node->name);
    if (function && !function->visible) {
      visitFunctionStmt(function);
    }
  }

  void visitAssignmentStmt(ast::AssignmentStmt* node) {
    ast::VariableExpr* variable = static_cast<ast::VariableExpr*>(node->variable);
    ast::VariableStmt* var = ast->findGlobalVariable(variable->name);
    if (var) {
      var->visible = true;
    }
    Visitor::visitAssignmentStmt(node);
  }

  void visitCallExpr(ast::CallExpr* node) override {
    Visitor::visitCallExpr(node);
    ast::FunctionStmt* function = ast->findFunction(node->name);
    if (function && !function->visible) {
      visitFunctionStmt(function);
    }
  }

  void visitVariableExpr(ast::VariableExpr* node) override {
    Visitor::visitVariableExpr(node);
    ast::VariableStmt* variable = ast->findGlobalVariable(node->name);
    if (variable && !variable->visible) {
      variable->visible = true;
      visitVariableStmt(variable);
    }
  }

  void visitType(ast::Type* type) override {
    if (type->nodeType == ast::NodeType::StructStmt) {
      ast::StructStmt* structStmt = ast->findStruct(type->name);
      if (structStmt && !structStmt->visible) {
        structStmt->visible = true;
        visitStructStmt(structStmt);
      }
    }
  }
};

}  // namespace visitor
