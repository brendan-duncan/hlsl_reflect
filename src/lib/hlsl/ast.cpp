#include "ast.h"

namespace hlsl {

Ast::Ast(Allocator* allocator)
  : _ownsAllocator(allocator == nullptr)
  , _allocator(allocator ? allocator : new Allocator()) {

  _firstPage = _allocator->alloc<NodePage>();
  _firstPage->next = nullptr;
  _currentPage = _firstPage;
  _currentPageOffset = 0;

  _root = createNode<AstRoot>();
}

Ast::~Ast() {
  NodePage* page = _firstPage;
  while (page != nullptr) {
    NodePage* next = page->next;
    _allocator->free(page);
    page = next;
  }
  if (_ownsAllocator) {
    delete _allocator;
  }
}

void* Ast::allocateMemory(size_t size) {
  if (_currentPageOffset + size > nodePageSize) {
    allocatePage();
  }
  
  void* buffer = _currentPage->buffer + _currentPageOffset;
  _currentPageOffset += size;
  return buffer;
}

void Ast::allocatePage() {
  NodePage* newPage = _allocator->alloc<NodePage>();
  newPage->next = nullptr;
  _currentPage->next = newPage;
  _currentPageOffset = 0;
  _currentPage = newPage;
}

} // namespace hlsl
