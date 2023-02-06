#include "ast/ast_node.h"

namespace hlsl {

class Visitor {
public:
  virtual void visitRoot(AstRoot* node);

  virtual void visitTopLevelStatement(AstStatement* node);
  virtual void visitStatements(AstStatement* node);
  
  virtual void visitStatement(AstStatement* node);
  virtual void visitBlock(AstBlock* node);
  virtual void visitBreakStmt(AstBreakStmt* node);
  virtual void visitBufferStmt(AstBufferStmt* node);
  virtual void visitContinueStmt(AstContinueStmt* node);
  virtual void visitDiscardStmt(AstDiscardStmt* node);
  virtual void visitDoWhileStmt(AstDoWhileStmt* node);
  virtual void visitExpressionStmt(AstExpressionStmt* node);
  virtual void visitForStmt(AstForStmt* node);
  virtual void visitFunctionStmt(AstFunctionStmt* node);
  virtual void visitIfStmt(AstIfStmt* node);
  virtual void visitStructStmt(AstStructStmt* node);
  virtual void visitSwitchStmt(AstSwitchStmt* node);
  virtual void visitVariableStmt(AstVariableStmt* node);
  virtual void visitTypedefStmt(AstTypedefStmt* node);
  virtual void visitWhileStmt(AstWhileStmt* node);
  virtual void visitCallStmt(AstCallStmt* node);
  virtual void visitAssignmentStmt(AstAssignmentStmt* node);

  virtual void visitExpression(AstExpression* node);  
  virtual void visitBinaryExpr(AstBinaryExpr* node);
  virtual void visitCallExpr(AstCallExpr* node);
  virtual void visitLiteralExpr(AstLiteralExpr* node);
  virtual void visitCastExpr(AstCastExpr* node);
  virtual void visitParameter(AstParameter* node);
  virtual void visitParameters(AstParameter* node);
  virtual void visitReturn(AstReturnStmt* node); 
  virtual void visitStringExpr(AstStringExpr* node);
  virtual void visitTernaryExpr(AstTernaryExpr* node);
  virtual void visitPrefixExpr(AstPrefixExpr* node);  
  virtual void visitVariableExpr(AstVariableExpr* node);
  virtual void visitIncrementExpr(AstIncrementExpr* node);
  virtual void visitArrayExpr(AstArrayExpr* node);
  virtual void visitMemberExpr(AstMemberExpr* node);
  virtual void visitAssignmentExpr(AstAssignmentExpr* node);
  virtual void visitArrayInitializerExpr(AstArrayInitializerExpr* node);
  virtual void visitStructInitializerExpr(AstStructInitializerExpr* node);

  virtual void visitArgument(AstExpression* node);
  virtual void visitArguments(AstExpression* node);
  virtual void visitAttribute(AstAttribute* node);
  virtual void visitAttributes(AstAttribute* node);
  virtual void visitBufferField(AstField* node);
  virtual void visitStateAssignment(AstStateAssignment* node);
  virtual void visitSamplerState(AstSamplerState* node);
  virtual void visitStructField(AstField* node);
  virtual void visitSwitchCase(AstSwitchCase* node);
  virtual void visitType(AstType* type);
};

} // namespace hlsl
