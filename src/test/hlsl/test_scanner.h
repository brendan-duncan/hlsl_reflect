#define _CRT_SECURE_NO_WARNINGS
#include <filesystem>

#include "../../lib/hlsl/scanner.h"
#include "../test.h"

using namespace hlsl;

namespace scanner_tests {

static Test test_Scanner("Scanner", []() {
  auto tokens = Scanner("double x = -0.6473313946860445;").scan();
  TEST_EQUALS(tokens.size(), 5ull);
  auto tIter = tokens.begin();
  TEST_EQUALS((*tIter).type(), TokenType::Double);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Identifier);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Equal);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::FloatLiteral);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Semicolon);
});

static Test test_Scanner_2("Scanner hex", []() {
  auto tokens = Scanner("0x0;").scan();
  TEST_EQUALS(tokens.size(), 2ull);
  auto tIter = tokens.begin();
  TEST_EQUALS((*tIter).type(), TokenType::IntLiteral);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Semicolon);
});

static Test test_Scanner_struct_scan("Scanner struct scan", []() {
  auto tokens = Scanner("struct foo { };").scan();
  TEST_EQUALS(tokens.size(), (size_t)5);
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
  TEST_EQUALS(tokens.size(), 2ull);
  auto tIter = tokens.begin();
  TEST_EQUALS((*tIter).lexeme(), "foo");
  tIter++;
  TEST_EQUALS((*tIter).lexeme(), "bar");
});

static Test test_comments("Scanner comments", []() {
  auto scanner = Scanner(R"(/* foo */
  foo
  // comment
  bar)");
  auto tokens = scanner.scan();
  TEST_EQUALS(tokens.size(), 2ull);
  auto tIter = tokens.begin();
  TEST_EQUALS((*tIter).lexeme(), "foo");
  tIter++;
  TEST_EQUALS((*tIter).lexeme(), "bar");
});

static Test test_string_literals("Scanner string literals", []() {
  auto scanner = Scanner(R"(string foo = "foo";)");
  auto tokens = scanner.scan();
  TEST_EQUALS(tokens.size(), 5ull);
  auto tIter = tokens.begin();
  TEST_EQUALS((*tIter).type(), TokenType::String);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Identifier);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Equal);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::StringLiteral);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Semicolon);
});

static Test test_define("Scanner define", []() {
  auto scanner = Scanner(R"(#define FOO float c = 1 + 2
  FOO;)");
  auto tokens = scanner.scan();
  // This should have expanded the FOO define and parsed as float c = 1 + 2 ;
  TEST_EQUALS(tokens.size(), 7ull);
  auto tIter = tokens.begin();
  TEST_EQUALS((*tIter).type(), TokenType::Float);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Identifier);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Equal);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::IntLiteral);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Plus);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::IntLiteral);
  tIter++;
  TEST_EQUALS((*tIter).type(), TokenType::Semicolon);
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
