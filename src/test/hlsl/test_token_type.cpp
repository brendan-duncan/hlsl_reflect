#include "../../lib/hlsl/token_type.h"
#include "../test.h"

namespace hlsl {

void testTokenType(Test& test) {
  test.test("TokenType", [](Test& test) {
    test.equals(findTokenType("123"), TokenType::IntLiteral);
    test.equals(findTokenType("123u"), TokenType::IntLiteral);
    test.equals(findTokenType("123U"), TokenType::IntLiteral);
    test.equals(findTokenType("123L"), TokenType::IntLiteral);
    test.equals(findTokenType("0123"), TokenType::IntLiteral);
    test.equals(findTokenType("0x123"), TokenType::IntLiteral);

    test.equals(findTokenType("1."), TokenType::FloatLiteral);
    test.equals(findTokenType("1.0"), TokenType::FloatLiteral);
    test.equals(findTokenType("1.0f"), TokenType::FloatLiteral);
    test.equals(findTokenType("-0.6473313946860445"), TokenType::FloatLiteral);
  });
}

} // namespace hlsl
