#pragma once

#include <iostream>
#include <string>

class Test {
public:
  void test(const std::string& s, void (*func)(Test&)) {
    _currentTest = s;
    _testIndex = 0;
    func(*this);
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

private:
  int _testIndex;
  std::string _currentTest;
};
