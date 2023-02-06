#define _CRT_SECURE_NO_WARNINGS
#include <filesystem>

#include "../../lib/hlsl/scanner.h"
#include "../test.h"

using namespace hlsl;

namespace scanner_tests {

static Test test_Scanner("Scanner", []() {
  auto tokens = Scanner("double x = -0.6473313946860445;").scan();
  TEST_EQUALS(tokens.size(), 6ull);
  TEST_EQUALS(tokens[0].type(), TokenType::Double);
  TEST_EQUALS(tokens[1].type(), TokenType::Identifier);
  TEST_EQUALS(tokens[2].type(), TokenType::Equal);
  TEST_EQUALS(tokens[3].type(), TokenType::FloatLiteral);
  TEST_EQUALS(tokens[4].type(), TokenType::Semicolon);
  TEST_EQUALS(tokens[5].type(), TokenType::EndOfFile);
});

static Test test_Scanner_2("Scanner hex", []() {
  auto tokens = Scanner("0x0;").scan();
  TEST_EQUALS(tokens.size(), 3ull);
  TEST_EQUALS(tokens[0].type(), TokenType::IntLiteral);
  TEST_EQUALS(tokens[1].type(), TokenType::Semicolon);
  TEST_EQUALS(tokens[2].type(), TokenType::EndOfFile);
});

static Test test_Scanner_struct_scan("Scanner struct scan", []() {
  auto tokens = Scanner("struct foo { };").scan();
  TEST_EQUALS(tokens.size(), (size_t)6);
});

static Test test_Scanner_struct_scanNext("Scanner struct scanNext", []() {
  auto scanner = Scanner(R"(struct foo { };)");
  size_t count = 0;
  while (!scanner.isAtEnd()) {
    auto tk = scanner.scanNext();
    count++;
  }
  TEST_EQUALS(count, 5ull);
});

static Test test_pragma("Scanner pragma", []() {
  auto scanner = Scanner(R"(#line 1
  foo
  #line 2 "foo"
  bar)");
  auto tokens = scanner.scan();
  TEST_EQUALS(tokens.size(), 3ull);
  TEST_EQUALS(tokens[0].lexeme(), "foo");
  TEST_EQUALS(tokens[1].lexeme(), "bar");
});

static Test test_comments("Scanner comments", []() {
  auto scanner = Scanner(R"(/* foo */
  foo
  // comment
  bar)");
  auto tokens = scanner.scan();
  TEST_EQUALS(tokens.size(), 3ull);
  TEST_EQUALS(tokens[0].lexeme(), "foo");
  TEST_EQUALS(tokens[1].lexeme(), "bar");
});

static Test test_string_literals("Scanner string literals", []() {
  auto scanner = Scanner(R"(string foo = "foo";)");
  auto tokens = scanner.scan();
  TEST_EQUALS(tokens.size(), 6ull);
  TEST_EQUALS(tokens[0].type(), TokenType::String);
  TEST_EQUALS(tokens[1].type(), TokenType::Identifier);
  TEST_EQUALS(tokens[2].type(), TokenType::Equal);
  TEST_EQUALS(tokens[3].type(), TokenType::StringLiteral);
  TEST_EQUALS(tokens[4].type(), TokenType::Semicolon);
});

static Test test_Shader("Scanner Shader", []() {
  FILE* fp = fopen(TEST_DATA_PATH("/hlsl/urp_bloom.hlsl"), "rb");
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
  TEST_EQUALS(count, 25901ull);

  free(hlsl);
});

} // namespace scanner_tests
