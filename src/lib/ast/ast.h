#pragma once

#include <string_view>

#include "../util/allocator.h"
#include "ast_node.h"

namespace ast {

/// Abstract Syntax Tree for parsed HLSL code.
/// All nodes are allocated from a memory pool, so the Ast owns the memory of all AstNodes it
/// contains.
class Ast {
public:
  Ast(util::Allocator* allocator = nullptr);

  ~Ast();

  /// The root node of the AST containing all top level statements.
  Root* root() const { return _root; }

  /// Used by the parser to create Ast nodes using the memory pool owned by the Ast.
  /// Creates a new node of type T, using a memory pool to allocate the memory
  /// @tparam T An AstNode derived type. This should have a static const AstNodeType astType member.
  /// @return T* The newly created node
  template<typename T>
  T* createNode() {
    Node* n = new (allocateMemory(sizeof(T))) T();
    n->nodeType = T::astType;
    return static_cast<T*>(n);
  }

private:
  void* allocateMemory(size_t size);

  void allocatePage();

  static const size_t nodePageSize = 1024 * 4;

  struct NodePage {
    NodePage* next;
    uint8_t buffer[nodePageSize];
  };

  bool _ownsAllocator;
  util::Allocator* _allocator;
  NodePage* _firstPage;
  NodePage* _currentPage;
  size_t _currentPageOffset;

  Root* _root;
};

} // namespace ast
