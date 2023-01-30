#pragma once

#include <string_view>

#include "../../util/allocator.h"
#include "ast_node.h"

namespace hlsl {

class Ast {
public:
  Ast(Allocator* allocator = nullptr);

  ~Ast();

  AstRoot* root() const { return _root; }

  template<typename T>
  T* createNode() {
    AstNode* n = new (allocateMemory(sizeof(T))) T();
    n->nodeType = T::type;
    return static_cast<T*>(n);
  }

private:
  void* allocateMemory(size_t size);

  void  allocatePage();

  static const size_t nodePageSize = 1024 * 4;

  struct NodePage {
    NodePage* next;
    uint8_t buffer[nodePageSize];
  };

  bool _ownsAllocator;
  Allocator* _allocator;
  NodePage* _firstPage;
  NodePage* _currentPage;
  size_t _currentPageOffset;

  AstRoot* _root;
};

} // namespace hlsl
