#include <fstream>
#include <iostream>
#include <fstream>
#include <memory>

#include "../lib/reader/hlsl/parser.h"

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

  std::string src;
  std::getline(fp, src, '\0');
  fp.close();

  std::unique_ptr<ast::Ast> ast{ reader::hlsl::Parser(src).parse() };

  if (!ast) {
    std::cerr << "Unable to parse file: " << argv[1] << std::endl;
    return 1;
  }

  return 0;
}
