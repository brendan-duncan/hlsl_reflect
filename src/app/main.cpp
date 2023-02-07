#include <fstream>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>

#include "../lib/hlsl/parser.h"
#include "../lib/hlsl/print_visitor.h"

typedef std::map<std::string, size_t> BufferFieldSizeMap;
typedef std::map<std::string, BufferFieldSizeMap> BufferMap;

class BufferFieldSizeVisitor : public hlsl::Visitor {
public:
  BufferMap bufferMap;
  BufferFieldSizeMap* _fieldSizeMap = nullptr;

  void visitBufferStmt(hlsl::AstBufferStmt* node) override {
    BufferFieldSizeMap fieldSizeMap;
    BufferFieldSizeMap* origFieldSizeMap = _fieldSizeMap;
    _fieldSizeMap = &fieldSizeMap;
    Visitor::visitBufferStmt(node);
    std::string bufferName = std::string(node->name);
    bufferMap[bufferName] = fieldSizeMap;
    _fieldSizeMap = origFieldSizeMap;
  }

  void visitBufferField(hlsl::AstField* node) override {
    (*_fieldSizeMap)[std::string(node->name)] = 0; // calculate field size, offset, etc.
  }
};

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

  BufferFieldSizeVisitor findBuffers;
  findBuffers.visitRoot(ast->root());

  for (auto& buffer : findBuffers.bufferMap) {
    std::cout << buffer.first << std::endl;
    for (auto& field : buffer.second) {
      std::string fieldName = field.first;
      size_t& fieldSize = field.second;
      std::cout << "  " << fieldName << " : " << fieldSize << std::endl;
    }
  }

  //hlsl::PrintVisitor visitor;
  //visitor.visitRoot(ast->root());
  //std::cout << std::flush;

  delete ast;

  return 0;
}
