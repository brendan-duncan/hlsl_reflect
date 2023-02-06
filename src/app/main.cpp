#include <iostream>

#include "../lib/hlsl/parser.h"
#include "../lib/hlsl/print_visitor.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: hlsl_reflect <file>" << std::endl;
    return 1;
  }

  FILE* fp = fopen(argv[1], "rb");
  if (fp == nullptr) {
    std::cerr << "Unable to open file: " << argv[1] << std::endl;
    return 1;
  }

  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char* hlsl = (char*)calloc(1, size + 1);
  fread(hlsl, size, 1, fp);
  fclose(fp);

  hlsl::Parser parser(hlsl);
  hlsl::Ast* ast = parser.parse();

  if (ast == nullptr) {
    free(hlsl);
    std::cerr << "Unable to parse file: " << argv[1] << std::endl;
    return 1;
  }

  hlsl::PrintVisitor visitor;
  visitor.visitRoot(ast->root());
  std::cout << std::flush;

  delete ast;
  free(hlsl);

  return 0;
}
