#include "test.h"
#include "hlsl/test_token_type.h"
#include "hlsl/test_scanner.h"
#include "hlsl/test_parser.h"
#include <iostream>
#include <chrono>

int main() {
  typedef std::chrono::high_resolution_clock Clock;
  auto t1 = Clock::now();
  Test::runPendingTests();
  auto t2 = Clock::now();
  std::cout << "!!!!" << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << '\n';
  return Test::getResults();
}
