#include "../../lib/reader/hlsl/token_type.h"
#include "../test.h"

using namespace reader::hlsl;

namespace token_type_tests {

static Test test_TokenType("TokenType IntLiteral", []() {
  TEST_EQUALS(findTokenType("123"), TokenType::IntLiteral);
  TEST_EQUALS(findTokenType("123u"), TokenType::IntLiteral);
  TEST_EQUALS(findTokenType("123U"), TokenType::IntLiteral);
  TEST_EQUALS(findTokenType("123L"), TokenType::IntLiteral);
  TEST_EQUALS(findTokenType("0123"), TokenType::IntLiteral);
  TEST_EQUALS(findTokenType("0x123"), TokenType::IntLiteral);
});

static Test test_TokenType_2("TokenType FloatLiteral", []() {
  TEST_EQUALS(findTokenType("1e-6"), TokenType::FloatLiteral);
  TEST_EQUALS(findTokenType("1."), TokenType::FloatLiteral);
  TEST_EQUALS(findTokenType("1.0"), TokenType::FloatLiteral);
  TEST_EQUALS(findTokenType("1.0f"), TokenType::FloatLiteral);
  TEST_EQUALS(findTokenType("-0.6473313946860445"), TokenType::FloatLiteral);
  TEST_EQUALS(findTokenType("4.8828125e-4"), TokenType::FloatLiteral);
});

} // namespace token_type_tests
