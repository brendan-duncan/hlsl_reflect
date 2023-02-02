#include "ast/ast_node.h"

namespace hlsl {

class Visitor {
public:
  virtual void visitRoot(AstRoot* node);

  virtual void visitTopLevelStatement(AstStatement* node);
  virtual void visitStatements(AstStatement* node);
  virtual void visitArgument(AstExpression* node);
  virtual void visitArguments(AstExpression* node);
  virtual void visitAssignment(AstAssignment* node);
  virtual void visitAttribute(AstAttribute* node);
  virtual void visitAttributes(AstAttribute* node);
  virtual void visitBinaryOperator(AstBinaryOperator* node);
  virtual void visitBlock(AstBlock* node);
  virtual void visitBreak(AstBreak* node);
  virtual void visitBuffer(AstBuffer* node);
  virtual void visitBufferField(AstBufferField* node);
  virtual void visitCallExpr(AstCallExpr* node);
  virtual void visitContinue(AstContinue* node);
  virtual void visitDiscard(AstDiscard* node);
  virtual void visitDoWhile(AstDoWhile* node);
  virtual void visitExpression(AstExpression* node);
  virtual void visitExpressionStatement(AstExpressionStatement* node);
  virtual void visitFor(AstFor* node);
  virtual void visitFunction(AstFunction* node);
  virtual void visitIf(AstIf* node);
  virtual void visitLiteralExpr(AstLiteralExpr* node);
  virtual void visitCastExpr(AstCastExpr* node);
  virtual void visitParameter(AstParameter* node);
  virtual void visitParameters(AstParameter* node);
  virtual void visitReturn(AstReturn* node);
  virtual void visitSamplerState(AstSamplerState* node);
  virtual void visitStateAssignment(AstStateAssignment* node);
  virtual void visitStatement(AstStatement* node);
  virtual void visitStringExpr(AstStringExpr* node);
  virtual void visitStruct(AstStruct* node);
  virtual void visitStructField(AstStructField* node);
  virtual void visitSwitch(AstSwitch* node);
  virtual void visitSwitchCase(AstSwitchCase* node);
  virtual void visitTernaryOperator(AstTernaryOperator* node);
  virtual void visitType(AstType* type);
  virtual void visitUnaryOperator(AstUnaryOperator* node);
  virtual void visitVariable(AstVariable* node);
  virtual void visitVariableExpr(AstVariableExpr* node);
  virtual void visitWhile(AstWhile* node);
  virtual void visitCall(AstCall* node);
};

} // namespace hlsl
