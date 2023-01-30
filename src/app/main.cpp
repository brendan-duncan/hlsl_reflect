#include <iostream>

#include "../lib/hlsl/scanner.h"

int main(int argc, char** argv) {
  auto tokens = hlsl::Scanner("double x = -0.6473313946860445;").scan();
  for (auto token : tokens) {
    std::cout << token.toString() << std::endl;
  }
  return 0;
}
