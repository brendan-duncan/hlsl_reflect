#pragma once

#include "visibility_visitor.h"

namespace visitor {

class PruneTree {
public:
  ast::Ast* ast;

  PruneTree(ast::Ast* ast);

  void prune(const std::string_view& entryPoint);
};

} // namespace visitor
