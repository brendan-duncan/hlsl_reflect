#pragma once

#include <list>
#include <map>
#include <string_view>

#include "../../ast/ast.h"
#include "../../ast/base_type.h"
#include "scanner.h"
#include "token.h"
#include "token_to_ast.h"

namespace hlsl {

/// The parser is responsible for taking the tokens from the scanner and building an ast::.
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
  /// @return ast::* The resulting Ast object.
  ast::Ast* parse();

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

  /// Push a token back onto the front of the pending token list.
  void pushBack(const Token& token) {
    _pending.push_front(token);
  }

  ast::Statement* parseTopLevelStatement();

  ast::StructStmt* parseStruct();

  ast::Field* parseStructField();

  ast::Attribute* parseAttributes();

  ast::Field* parseBufferField();

  ast::SamplerState* parseSamplerState();
  
  ast::TypedefStmt* parseTypedef();

  ast::StateAssignment* parseStateAssignment(bool isSamplerState, bool isPipelineState);

  ast::BufferStmt* parseBuffer();

  ast::Type* parseType(bool allowVoid, const char* exceptionMessage = nullptr);

  bool parseTypeModifier(uint32_t& flags);

  bool parseInterpolationModifier(uint32_t& flags);

  ast::Expression* parseAssignmentExpression(ast::Type* type);

  ast::Expression* parseStructInitialization(ast::Type* type);

  ast::Expression* parseArrayInitialization(ast::Type* type);

  ast::Expression* parseExpression();

  ast::Expression* parseExpressionList();

  ast::Expression* parseLogicalOrExpression();

  ast::Expression* parseLogicalAndExpression();

  ast::Expression* parseInclusiveOrExpression();

  ast::Expression* parseExclusiveOrExpression();

  ast::Expression* parseAndExpression();

  ast::Expression* parseEqualityExpression();

  ast::Expression* parseRelationalExpression();

  ast::Expression* parseShiftExpression();

  ast::Expression* parseAdditiveExpression();

  ast::Expression* parseMultiplicativeExpression();

  ast::Expression* parsePrefixExpression();

  ast::Expression* parseSingularExpression();

  ast::Expression* parsePostfixExpression(ast::Expression* expr);

  ast::Expression* parsePrimaryExpression();

  ast::Expression* parseParenthesizedExpression();

  ast::Expression* parseArgumentList();

  ast::FunctionStmt* parseFunctionStmt(ast::Type* returnType, const std::string_view& name);

  ast::VariableStmt* parseVariableStmt(ast::Type* type, const std::string_view& name,
    ast::Attribute* attributes);

  ast::Parameter* parseParameterList();

  ast::Parameter* parseParameter();

  ast::Block* parseBlock();

  ast::Statement* parseStatement(bool expectSemicolon = true);

  ast::IfStmt* parseIfStmt();

  ast::SwitchStmt* parseSwitchStmt();

  ast::ForStmt* parseForStmt();

  ast::DoWhileStmt* parseDoWhileStmt();

  ast::WhileStmt* parseWhileStmt();

  ast::Expression* parseArraySize();

  /// Returns true if the token is a type name, either built-in, user defined, or a struct
  bool isType(const Token& tk) {
    return tokenTypeToBaseType(tk.type()) != ast::BaseType::Undefined ||
            _typedefs.find(tk.lexeme()) != _typedefs.end() ||
            _structs.find(tk.lexeme()) != _structs.end();
  }

  // Start saving all tokens that are consumed. restorePoint() can be called to restore the tokens
  // that were consumed back to the startRestorePoint. This is used to undo a parse, since some
  // grammar rules are ambiguous.
  void startRestorePoint() {
    _restore.push_back(std::list<Token>());
  }

  // Restore the tokens that were recorded since the last startRestorePoint() call.
  void restorePoint() {
    if (_restore.empty()) {
      return;
    }

    std::list<Token>& restore = _restore.back();
    _pending.splice(_pending.begin(), restore);
    _restore.pop_back();
  }

  // Discard the tokens that were recorded since the last startRestorePoint() call.
  void discardRestorePoint() {
    _restore.pop_back();   
  }

  // The AST being constructed.
  ast::Ast* _ast = nullptr;
  // The lexer that is used to scan the source string into Tokens.
  Scanner _scanner;
  // A list of tokens that have been scanned from the scanner but not yet parsed.
  std::list<Token> _pending;

  //std::list<size_t> _restorePointStack;
  std::list<std::list<Token>> _restore;

  // Track typedefs to verify type names.
  std::map<std::string_view, ast::TypedefStmt*> _typedefs;
  // Track structs to verify type names.
  std::map<std::string_view, ast::StructStmt*> _structs;
  std::map<std::string_view, ast::VariableStmt*> _variables;
  std::list<std::string> _anonymousStructNames;
};

} // namespace hlsl
