#include "prune_tree.h"

namespace visitor {

PruneTree::PruneTree(ast::Ast* ast)
    : ast(ast) {
}

void PruneTree::prune(const std::string_view& entryPoint) {
  ast::FunctionStmt* entry = ast->findFunction(entryPoint);
  if (entry != nullptr) {
    MarkVisibleStatements markVisibleStatements(ast);
    markVisibleStatements.visitFunctionStmt(entry);
  }

  ast::Statement* stmt = ast->root()->statements;
  while (stmt != nullptr) {
    if (stmt->nodeType == ast::NodeType::BufferStmt) {
      ast::BufferStmt* buffer = static_cast<ast::BufferStmt*>(stmt);
      if (buffer->visible) {
        stmt = stmt->next;
        continue;
      }
      ast::Field* field = buffer->field;
      while (field != nullptr) {
        if (field->visible) {
          buffer->visible = true;
          break;
        }
        field = field->next;
      }
    }
    stmt = stmt->next;
  }
}

} // namespace visitor
