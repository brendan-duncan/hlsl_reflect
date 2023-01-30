#include "test.h"

namespace hlsl {
  void testParser(Test& test);
  void testScanner(Test& test);
  void testTokenType(Test& test);
}

int main() {
  Test test;
  hlsl::testTokenType(test);
  hlsl::testScanner(test);
  hlsl::testParser(test);
  return 0;
}
