#include "test.h"
#include "hlsl/test_scanner.h"
#include "hlsl/test_token_type.h"
#include "hlsl/test_parser.h"

int main() {
  Test::runPendingTests();
  return Test::getResults();
}
