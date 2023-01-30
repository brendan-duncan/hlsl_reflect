#pragma once

#include <iostream>
#include <string>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

class Test {
public:
  void test(const std::string& s, void (*func)(Test&)) {
    _currentTest = s;
    _testIndex = 0;
    try {
      func(*this);
    } catch (...) {
      std::cout << "[FAIL] " << _currentTest << "[" << _testIndex << "]" << std::endl;
    }
  }

  template<typename T>
  inline bool equals(T a, T b) {
    if (a != b) {
      std::cout << "[FAIL] " << _currentTest << "[" << _testIndex << "]: " << a << " != " 
          << b << std::endl;
      _testIndex++;
      return false;
    }
    std::cout << "[PASS] " << _currentTest << "[" << _testIndex << "]" << std::endl;
    _testIndex++;
    return true;
  }

  inline bool isNotNull(void* p) {
    if (p == nullptr) {
      std::cout << "[FAIL] " << _currentTest << "[" << _testIndex << "]" << std::endl;
      _testIndex++;
      return false;
    }
    std::cout << "[PASS] " << _currentTest << "[" << _testIndex << "]" << std::endl;
    _testIndex++;
    return true;
  }

  static const char* dataPath() {
    return TOSTRING(DATA_PATH);
  }

  static const std::string dataPath(const char* path) {
    return std::string(dataPath()) + path;
  }

private:
  int _testIndex;
  std::string _currentTest;
};
