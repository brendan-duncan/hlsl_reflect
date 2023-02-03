#define _CRT_SECURE_NO_WARNINGS
#include <filesystem>

#include "../../lib/hlsl/scanner.h"
#include "../test.h"

using namespace hlsl;

TEST(Scanner, []() {
  auto tokens = Scanner("double x = -0.6473313946860445;").scan();
  TEST_EQUALS(tokens.size(), 6ull);
  TEST_EQUALS(tokens[0].type(), TokenType::Double);
  TEST_EQUALS(tokens[1].type(), TokenType::Identifier);
  TEST_EQUALS(tokens[2].type(), TokenType::Equal);
  TEST_EQUALS(tokens[3].type(), TokenType::FloatLiteral);
  TEST_EQUALS(tokens[4].type(), TokenType::Semicolon);
  TEST_EQUALS(tokens[5].type(), TokenType::EndOfFile);
});

static Test test_Scanner_2("Scanner_2", []() {
  auto tokens = Scanner("0x0;").scan();
  TEST_EQUALS(tokens.size(), 3ull);
  TEST_EQUALS(tokens[0].type(), TokenType::IntLiteral);
  TEST_EQUALS(tokens[1].type(), TokenType::Semicolon);
  TEST_EQUALS(tokens[2].type(), TokenType::EndOfFile);
});

TEST(Scanner_struct_scan, []() {
  auto tokens = Scanner("struct foo { };").scan();
  TEST_EQUALS(tokens.size(), (size_t)6);
});

TEST(Scanner_struct_scanNext, []() {
  auto scanner = Scanner("struct foo { };");
  size_t count = 0;
  while (!scanner.isAtEnd()) {
    auto tk = scanner.scanNext();
    count++;
  }
  TEST_EQUALS(count, 5ull);
});

TEST(Shader, []() {
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
  TEST_EQUALS(count, 25925ull);

  free(hlsl);
});

