#include "test.h"
#include "test_scanner.h"
#include "test_token_type.h"

int main() {
  Test test;
  testTokenType(test);
  testScanner(test);
  return 0;
}
