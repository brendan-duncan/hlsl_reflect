#include "ast/ast_node.h"

namespace hlsl {

class Visitor {
public:
  virtual void visitRoot(AstRoot* node);

  virtual void visitTopLevelStatement(AstStatement* node);
  virtual void visitStatements(AstStatement* node);
  virtual void visitArgument(AstExpression* node);
  virtual void visitArguments(AstExpression* node);
  virtual void visitAssignment(AstAssignmentStmt* node);
  virtual void visitAttribute(AstAttribute* node);
  virtual void visitAttributes(AstAttribute* node);
  virtual void visitBinaryOperator(AstBinaryExpr* node);
  virtual void visitBlock(AstBlock* node);
  virtual void visitBreak(AstBreakStmt* node);
  virtual void visitBuffer(AstBufferStmt* node);
  virtual void visitBufferField(AstBufferField* node);
  virtual void visitCallExpr(AstCallExpr* node);
  virtual void visitContinue(AstContinueStmt* node);
  virtual void visitDiscard(AstDiscardStmt* node);
  virtual void visitDoWhile(AstDoWhileStmt* node);
  virtual void visitExpression(AstExpression* node);
  virtual void visitExpressionStatement(AstExpressionStmt* node);
  virtual void visitFor(AstForStmt* node);
  virtual void visitFunction(AstFunctionStmt* node);
  virtual void visitIf(AstIfStmt* node);
  virtual void visitLiteralExpr(AstLiteralExpr* node);
  virtual void visitCastExpr(AstCastExpr* node);
  virtual void visitParameter(AstParameter* node);
  virtual void visitParameters(AstParameter* node);
  virtual void visitReturn(AstReturnStmt* node);
  virtual void visitSamplerState(AstSamplerState* node);
  virtual void visitStateAssignment(AstStateAssignment* node);
  virtual void visitStatement(AstStatement* node);
  virtual void visitStringExpr(AstStringExpr* node);
  virtual void visitStruct(AstStructStmt* node);
  virtual void visitStructField(AstStructField* node);
  virtual void visitSwitch(AstSwitchStmt* node);
  virtual void visitSwitchCase(AstSwitchCase* node);
  virtual void visitTernaryOperator(AstTernaryExpr* node);
  virtual void visitType(AstType* type);
  virtual void visitUnaryOperator(AstUnaryExpr* node);
  virtual void visitVariable(AstVariableStmt* node);
  virtual void visitVariableExpr(AstVariableExpr* node);
  virtual void visitWhile(AstWhileStmt* node);
  virtual void visitCall(AstCallStmt* node);
};

} // namespace hlsl
