#pragma once

#include <list>
#include <string_view>

#include "ast.h"
#include "scanner.h"
#include "token.h"

namespace hlsl {

/// The parser is responsible for taking the tokens from the scanner and building an Ast.
class Parser {
public:
  /// Construct a new Parser object for the given source.
  /// The source string must be valid for the lifetime of the parser and resulting Ast object, as
  /// all string values are views of the source string.
  /// @param source The source string to parse.
  Parser(const std::string_view& source);

  /// Parse the source string and return the resulting Ast object.
  /// @return Ast* The resulting Ast object.
  Ast* parse();

private:
  // Returns true if the current token is at the end of the source.
  bool isAtEnd();

  // Advance t othe next token and error if it is not of the given type.
  // @param type The type of token expected.
  // @param message The message to display if the token is not of the given type.
  // @return The token that was consumed.
  Token consume(TokenType type, const char* message = nullptr);

  // Advance to the next token and return the previous token.
  // @return The previous token.
  Token advance();

  // Return the next token without advancing to the next token.
  const Token& peekNext();

  // Advance to the next token only if the next token is the given type.
  // @param type The type of token to match.
  // @return true if the next token is the given type.
  bool match(TokenType type);

  // Check if the next token is the given type.
  // @param type The type of token to match.
  // @return true if the next token is the given type.
  bool check(TokenType type);

  AstStatement* parseTopLevelStatement();

  AstStruct* parseStruct();

  AstStructField* parseStructField();

  AstAttribute* parseAttributes();

  AstExpression* parseExpression();

  AstExpression* parseExpressionList();

  AstDeclaration* parseDeclaration();

  AstSamplerState* parseSamplerState();

  AstStateAssignment* parseStateAssignment(bool isSamplerState, bool isPipelineState);

  AstBuffer* parseBuffer();

  AstType* parseType(bool allowVoid);

  bool parseTypeModifier(TypeFlags& flags);

  bool parseInterpolationModifier(TypeFlags& flags);

  AstExpression* parseShortCircuitOrExpression();

  AstExpression* parseShortCircuitAndExpression();

  AstExpression* parseInclusiveOrExpression();

  AstExpression* parseExclusiveOrExpression();

  AstExpression* parseAndExpression();

  AstExpression* parseEqualityExpression();

  AstExpression* parseRelationalExpression();

  AstExpression* parseShiftExpression();

  AstExpression* parseAdditiveExpression();

  AstExpression* parseMultiplicativeExpression();

  AstExpression* parseUnaryExpression();

  AstExpression* parseSingularExpression();

  AstExpression* parsePostfixExpression();

  AstExpression* parsePrimaryExpression();

  AstExpression* parseParenthesizedExpression();

  AstExpression* parseArgumentList();

  AstFunction* parseFunction(AstType* returnType, const std::string_view& name);

  AstVariable* parseVariable(AstType* type, const std::string_view& name);

  AstParameter* parseParameterList();

  AstParameter* parseParameter();

  AstBlock* parseBlock();

  AstStatement* parseStatement();

  AstIf* parseIf();

  AstSwitch* parseSwitch();

  AstFor* parseFor();

  AstDoWhile* parseDoWhile();

  AstWhile* parseWhile();

  Ast* _ast = nullptr;
  Scanner _scanner;
  std::list<Token> _pending;
};

} // namespace hlsl
