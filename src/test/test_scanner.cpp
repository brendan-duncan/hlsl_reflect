#include <filesystem>
#include "../lib/scanner.h"
#include "test.h"

void testScanner(Test& test) {
  test.test("Scanner", [](Test& test) {
    auto tokens = hlsl::Scanner("double x = -0.6473313946860445;").scan();
    test.equals(tokens.size(), (size_t)6);
    test.equals(tokens[0].type(), hlsl::TokenType::Double);
    test.equals(tokens[1].type(), hlsl::TokenType::Identifier);
    test.equals(tokens[2].type(), hlsl::TokenType::Equal);
    test.equals(tokens[3].type(), hlsl::TokenType::FloatLiteral);
    test.equals(tokens[4].type(), hlsl::TokenType::Semicolon);
    test.equals(tokens[5].type(), hlsl::TokenType::EndOfFile);
  });

  test.test("Shader", [](Test& test) {
    FILE* fp = fopen("src/test/_data/urp_bloom.hlsl", "rb");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* hlsl = (char*)calloc(1, size + 1);
    fread(hlsl, size, 1, fp);
    hlsl[size] = '\0';
    fclose(fp);

    size_t count = 0;
    hlsl::Scanner scanner(hlsl);
    while (!scanner.isAtEnd()) {
      auto tk = scanner.scanNext();
      count++;
    }
    std::cout << count << std::endl;

    free(hlsl);
  });
}
