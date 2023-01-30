#define _CRT_SECURE_NO_WARNINGS
#include <filesystem>

#include "../../lib/hlsl/scanner.h"
#include "../test.h"

namespace hlsl {

void testScanner(Test& test) {
  test.test("Scanner", [](Test& test) {
    auto tokens = Scanner("double x = -0.6473313946860445;").scan();
    test.equals(tokens.size(), (size_t)6);
    test.equals(tokens[0].type(), TokenType::Double);
    test.equals(tokens[1].type(), TokenType::Identifier);
    test.equals(tokens[2].type(), TokenType::Equal);
    test.equals(tokens[3].type(), TokenType::FloatLiteral);
    test.equals(tokens[4].type(), TokenType::Semicolon);
    test.equals(tokens[5].type(), TokenType::EndOfFile);
  });

  test.test("Scanner struct scan", [](Test& test) {
    auto tokens = Scanner("struct foo { };").scan();
    test.equals(tokens.size(), (size_t)6);
  });

  test.test("Scanner struct scanNext", [](Test& test) {
    auto scanner = Scanner("struct foo { };");
    size_t count = 0;
    while (!scanner.isAtEnd()) {
      auto tk = scanner.scanNext();
      count++;
    }
    test.equals(count, 5ull);
  });

  test.test("Shader", [](Test& test) {
    FILE* fp = fopen(test.dataPath("/hlsl/urp_bloom.hlsl").c_str(), "rb");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* hlsl = (char*)calloc(1, size + 1);
    fread(hlsl, size, 1, fp);
    hlsl[size] = '\0';
    fclose(fp);

    size_t count = 0;
    Scanner scanner(hlsl);
    while (!scanner.isAtEnd()) {
      auto tk = scanner.scanNext();
      count++;
    }
    test.equals(count, 25882ull);

    free(hlsl);
  });
}

} // namespace hlsl
