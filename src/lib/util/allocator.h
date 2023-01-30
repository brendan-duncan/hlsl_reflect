#pragma once

namespace hlsl {

class Allocator {
public:
  template<typename T>
  T* alloc() {
    return (T*)malloc(sizeof(T));
  }

  template<typename T>
  T* alloc(size_t count) {
    return (T*)malloc(sizeof(T) * count);
  }

  template<typename T>
  void free(T* ptr) {
    ::free((void*)ptr);
  }

  template<typename T>
  T* realloc(T* ptr, size_t count) {
    return (T*)::realloc(ptr, sizeof(T) * count);
  }
};

} // namespace hlsl
