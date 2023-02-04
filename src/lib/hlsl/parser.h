#pragma once

#include <list>
#include <map>
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

  const std::string_view& source() { return _scanner.source(); }

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

  AstType* parseType(bool allowVoid, const char* exceptionMessage = nullptr);

  bool parseTypeModifier(uint32_t& flags);

  bool parseInterpolationModifier(uint32_t& flags);

  AstExpression* parseExpression();

  AstExpression* parseExpressionList();

  AstExpression* parseLogicalOrExpression();

  AstExpression* parseLogicalAndExpression();

  AstExpression* parseInclusiveOrExpression();

  AstExpression* parseExclusiveOrExpression();

  AstExpression* parseAndExpression();

  AstExpression* parseEqualityExpression();

  AstExpression* parseRelationalExpression();

  AstExpression* parseShiftExpression();

  AstExpression* parseAdditiveExpression();

  AstExpression* parseMultiplicativeExpression();

  AstExpression* parsePrefixExpression();

  AstExpression* parseSingularExpression();

  AstExpression* parsePostfixExpression(AstExpression* expr);

  AstExpression* parsePrimaryExpression();

  AstExpression* parseParenthesizedExpression();

  AstExpression* parseArgumentList();

  AstFunctionStmt* parseFunctionStmt(AstType* returnType, const std::string_view& name);

  AstVariableStmt* parseVariableStmt(AstType* type, const std::string_view& name);

  AstParameter* parseParameterList();

  AstParameter* parseParameter();

  AstBlock* parseBlock();

  AstStatement* parseStatement();

  AstIfStmt* parseIfStmt();

  AstSwitchStmt* parseSwitchStmt();

  AstForStmt* parseForStmt();

  AstDoWhileStmt* parseDoWhileStmt();

  AstWhileStmt* parseWhileStmt();

  bool isType(const Token& tk) {
    return tokenTypeToBaseType(tk.type()) != BaseType::Undefined ||
            _typedefs.find(tk.lexeme()) != _typedefs.end();
  }

  void startRestorePoint() {
    _restorePoint++;
  }

  void restore() {
    _restorePoint--;
    if (_restorePoint == 0) {
      _pending.splice(_pending.begin(), _restore);
      _restore.clear();
    }
  }

  void discardRestorePoint() {
    _restorePoint--;
    if (_restorePoint == 0) {
      _restore.clear();
    }
  }

  Ast* _ast = nullptr;
  Scanner _scanner;
  std::list<Token> _pending;

  int _restorePoint = 0;
  std::list<Token> _restore;

  // Track typedefs to verify type names.
  std::map<std::string_view, AstTypedefStmt*> _typedefs;
};

} // namespace hlsl
