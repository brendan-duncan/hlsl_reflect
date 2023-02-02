#include "../../lib/hlsl/token_type.h"
#include "../test.h"

using namespace hlsl;

TEST(TokenType, []() {
  TEST_EQUALS(findTokenType("123"), TokenType::IntLiteral);
  TEST_EQUALS(findTokenType("123u"), TokenType::IntLiteral);
  TEST_EQUALS(findTokenType("123U"), TokenType::IntLiteral);
  TEST_EQUALS(findTokenType("123L"), TokenType::IntLiteral);
  TEST_EQUALS(findTokenType("0123"), TokenType::IntLiteral);
  TEST_EQUALS(findTokenType("0x123"), TokenType::IntLiteral);
});

TEST(TokenType_2, []() {
  TEST_EQUALS(findTokenType("1."), TokenType::FloatLiteral);
  TEST_EQUALS(findTokenType("1.0"), TokenType::FloatLiteral);
  TEST_EQUALS(findTokenType("1.0f"), TokenType::FloatLiteral);
  TEST_EQUALS(findTokenType("-0.6473313946860445"), TokenType::FloatLiteral);
});
