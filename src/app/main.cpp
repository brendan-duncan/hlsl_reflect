#include <fstream>
#include <iostream>
#include <string>

#include "../lib/hlsl/parser.h"
#include "../lib/hlsl/print_visitor.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: hlsl_reflect <file>" << std::endl;
    return 1;
  }

  std::ifstream fp(argv[1], std::ios::binary);

  if (!fp) {
    std::cerr << "Unable to open file: " << argv[1] << std::endl;
    return 1;
  }

  std::string hlsl;
  std::getline(fp, hlsl, '\0');
  fp.close();

  hlsl::Parser parser(hlsl);
  hlsl::Ast* ast = parser.parse();

  if (ast == nullptr) {
    std::cerr << "Unable to parse file: " << argv[1] << std::endl;
    return 1;
  }

  hlsl::PrintVisitor visitor;
  visitor.visitRoot(ast->root());
  std::cout << std::flush;

  delete ast;

  return 0;
}
