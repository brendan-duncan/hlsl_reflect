#pragma once

#include <list>
#include <string_view>

#include "ast.h"
#include "scanner.h"
#include "token.h"

namespace hlsl {

/// The parser is responsible for taking the tokens from the scanner and building an Ast.
/// This is a recursive descent parser, which means that each method is responsible for parsing
/// a single grammar rule. HLSL does not have a formal specification defining its grammar,
/// so the grammar rules are based on the HLSL documentation.
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

  void pushBack(const Token& token) {
    _pending.push_front(token);
  }

  AstStatement* parseTopLevelStatement();

  AstStructStmt* parseStruct();

  AstStructField* parseStructField();

  AstAttribute* parseAttributes();

  AstBufferField* parseBufferField();

  AstSamplerState* parseSamplerState();
  
  AstTypedefStmt* parseTypedef();

  AstStateAssignment* parseStateAssignment(bool isSamplerState, bool isPipelineState);

  AstBufferStmt* parseBuffer();

  AstType* parseType(bool allowVoid);

  bool parseTypeModifier(TypeFlags& flags);

  bool parseInterpolationModifier(TypeFlags& flags);

  AstExpression* parseExpression();

  AstExpression* parseExpressionList();

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

  AstFunctionStmt* parseFunction(AstType* returnType, const std::string_view& name);

  AstVariableStmt* parseVariable(AstType* type, const std::string_view& name);

  AstParameter* parseParameterList();

  AstParameter* parseParameter();

  AstBlock* parseBlock();

  AstStatement* parseStatement();

  AstIfStmt* parseIf();

  AstSwitchStmt* parseSwitch();

  AstForStmt* parseFor();

  AstDoWhileStmt* parseDoWhile();

  AstWhileStmt* parseWhile();

  Ast* _ast = nullptr;
  Scanner _scanner;
  std::list<Token> _pending;
};

} // namespace hlsl
