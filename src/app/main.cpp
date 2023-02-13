#include <fstream>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>

#include "../lib/reader/hlsl/parser.h"
#include "../lib/visitor/print_visitor.h"

typedef std::map<std::string, size_t> BufferFieldSizeMap;
typedef std::map<std::string, BufferFieldSizeMap> BufferMap;

class BufferFieldSizeVisitor : public visitor::Visitor {
public:
  BufferMap bufferMap;
  BufferFieldSizeMap* _fieldSizeMap = nullptr;

  void visitBufferStmt(ast::BufferStmt* node) override {
    BufferFieldSizeMap fieldSizeMap;
    BufferFieldSizeMap* origFieldSizeMap = _fieldSizeMap;
    _fieldSizeMap = &fieldSizeMap;
    visitor::Visitor::visitBufferStmt(node);
    std::string bufferName = std::string(node->name);
    bufferMap[bufferName] = fieldSizeMap;
    _fieldSizeMap = origFieldSizeMap;
  }

  void visitBufferField(ast::Field* node) override {
    (*_fieldSizeMap)[std::string(node->name)] = 0; // calculate field size, offset, etc.
  }
};

int main(int argc, char** argv) {
#if 0
  size_t passed = 0;
  size_t failed = 0;
  const std::filesystem::path sandbox{"D:/src/unity_web_research/webgpu/wgsl/boat_attack"};
  for (auto const& dirEntry : std::filesystem::directory_iterator{sandbox}) {
    std::filesystem::path ext = dirEntry.path().extension();
    if (dirEntry.is_regular_file() && ext == ".hlsl") {
      std::string path = dirEntry.path().u8string();
      FILE* fp = fopen(path.c_str(), "rb");
      if (fp == nullptr) {
        std::cerr << "Unable to open file: " << path << std::endl;
        continue;
      }

      fseek(fp, 0, SEEK_END);
      size_t size = ftell(fp);
      fseek(fp, 0, SEEK_SET);
      char* hlsl = (char*)calloc(1, size + 1);
      fread(hlsl, size, 1, fp);
      fclose(fp);
    
      std::string fullname = "D:/ast/" + dirEntry.path().filename().u8string();
      size_t lastindex = fullname.find_last_of("."); 
      std::string astName = fullname.substr(0, lastindex) + ".ast"; 

      //std::cout << path << " -> " << astName << std::endl;

      hlsl::Parser parser(hlsl);
      hlsl::Ast* ast = parser.parse();

      if (ast == nullptr || ast->root() == nullptr) {
        free(hlsl);
        std::cerr << "Unable to parse file: " << path << std::endl;
        failed++;
        continue;
      }
     
      //std::ofstream out{astName};
      //hlsl::PrintVisitor visitor(out);
      //visitor.visitRoot(ast->root());
      passed++;

      delete ast;
      free(hlsl);
    }
  }
  std::cout << "PASSED: " << passed << " FAILED: " << failed << " / TOTAL: " << (passed + failed) << std::endl;
#endif // 0
#if 0
  const char* path = "D:/src/unity_web_research/webgpu/wgsl/boat_attack/unity_webgpu_000001651A602D90.fs.hlsl";
  FILE* fp = fopen(path, "rb");
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
    std::cerr << "Unable to parse file: " << path << std::endl;
    return 1;
  }

  hlsl::PrintVisitor visitor;
  visitor.visitRoot(ast->root());
  std::cout << std::flush;

  delete ast;
  free(hlsl);
#endif // 0
  
#if 1
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
  ast::Ast* ast = parser.parse();

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
#endif // 0
  return 0;
}
