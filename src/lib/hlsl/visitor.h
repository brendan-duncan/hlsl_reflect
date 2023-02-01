#include "ast/ast_node.h"

namespace hlsl {

class Visitor {
public:
  virtual void visitRoot(AstRoot* node);
  virtual void visitTopLevelStatement(AstStatement* node);
  virtual void visitType(TokenType type);
  virtual void visitStatements(AstStatement* node);
  virtual void visitStatement(AstStatement* node);
  virtual void visitDeclaration(AstDeclaration* node);
  virtual void visitStruct(AstStruct* node);
  virtual void visitStructField(AstStructField* node);
  virtual void visitBuffer(AstBuffer* node);
  virtual void visitFunction(AstFunction* node);
  virtual void visitParameter(AstParameter* node);
  virtual void visitExpressionStatement(AstExpressionStatement* node);
  virtual void visitExpression(AstExpression* node);
  virtual void visitReturn(AstReturn* node);
  virtual void visitIf(AstIf* node);
  virtual void visitFor(AstFor* node);
  virtual void visitDoWhile(AstDoWhile* node);
  virtual void visitWhile(AstWhile* node);
  virtual void visitSwitch(AstSwitch* node);
  virtual void visitSwitchCase(AstSwitchCase* node);
  virtual void visitDiscard(AstDiscard* node);
};

} // namespace hlsl
