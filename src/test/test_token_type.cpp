#include "../lib/token_type.h"
#include "test.h"

void testTokenType(Test& test) {
  test.test("TokenType", [](Test& test) {
    test.equals(hlsl::findTokenType("123"), hlsl::TokenType::IntLiteral);
    test.equals(hlsl::findTokenType("123u"), hlsl::TokenType::IntLiteral);
    test.equals(hlsl::findTokenType("123U"), hlsl::TokenType::IntLiteral);
    test.equals(hlsl::findTokenType("123L"), hlsl::TokenType::IntLiteral);
    test.equals(hlsl::findTokenType("0123"), hlsl::TokenType::IntLiteral);
    test.equals(hlsl::findTokenType("0x123"), hlsl::TokenType::IntLiteral);

    test.equals(hlsl::findTokenType("1."), hlsl::TokenType::FloatLiteral);
    test.equals(hlsl::findTokenType("1.0"), hlsl::TokenType::FloatLiteral);
    test.equals(hlsl::findTokenType("1.0f"), hlsl::TokenType::FloatLiteral);
    test.equals(hlsl::findTokenType("-0.6473313946860445"), hlsl::TokenType::FloatLiteral);
  });
}
