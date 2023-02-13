#include "../ast/ast_node.h"

namespace visitor {

class Visitor {
public:
  virtual void visitRoot(ast::AstRoot* node);

  virtual void visitTopLevelStatement(ast::AstStatement* node);
  virtual void visitStatements(ast::AstStatement* node);
  
  virtual void visitStatement(ast::AstStatement* node);
  virtual void visitBlock(ast::AstBlock* node);
  virtual void visitBreakStmt(ast::AstBreakStmt* node);
  virtual void visitBufferStmt(ast::AstBufferStmt* node);
  virtual void visitContinueStmt(ast::AstContinueStmt* node);
  virtual void visitDiscardStmt(ast::AstDiscardStmt* node);
  virtual void visitDoWhileStmt(ast::AstDoWhileStmt* node);
  virtual void visitExpressionStmt(ast::AstExpressionStmt* node);
  virtual void visitForStmt(ast::AstForStmt* node);
  virtual void visitFunctionStmt(ast::AstFunctionStmt* node);
  virtual void visitIfStmt(ast::AstIfStmt* node);
  virtual void visitStructStmt(ast::AstStructStmt* node);
  virtual void visitSwitchStmt(ast::AstSwitchStmt* node);
  virtual void visitVariableStmt(ast::AstVariableStmt* node);
  virtual void visitTypedefStmt(ast::AstTypedefStmt* node);
  virtual void visitWhileStmt(ast::AstWhileStmt* node);
  virtual void visitCallStmt(ast::AstCallStmt* node);
  virtual void visitAssignmentStmt(ast::AstAssignmentStmt* node);

  virtual void visitExpression(ast::AstExpression* node);  
  virtual void visitBinaryExpr(ast::AstBinaryExpr* node);
  virtual void visitCallExpr(ast::AstCallExpr* node);
  virtual void visitLiteralExpr(ast::AstLiteralExpr* node);
  virtual void visitCastExpr(ast::AstCastExpr* node);
  virtual void visitParameter(ast::AstParameter* node);
  virtual void visitParameters(ast::AstParameter* node);
  virtual void visitReturn(ast::AstReturnStmt* node); 
  virtual void visitStringExpr(ast::AstStringExpr* node);
  virtual void visitTernaryExpr(ast::AstTernaryExpr* node);
  virtual void visitPrefixExpr(ast::AstPrefixExpr* node);  
  virtual void visitVariableExpr(ast::AstVariableExpr* node);
  virtual void visitIncrementExpr(ast::AstIncrementExpr* node);
  virtual void visitArrayExpr(ast::AstArrayExpr* node);
  virtual void visitMemberExpr(ast::AstMemberExpr* node);
  virtual void visitAssignmentExpr(ast::AstAssignmentExpr* node);
  virtual void visitArrayInitializerExpr(ast::AstArrayInitializerExpr* node);
  virtual void visitStructInitializerExpr(ast::AstStructInitializerExpr* node);

  virtual void visitArgument(ast::AstExpression* node);
  virtual void visitArguments(ast::AstExpression* node);
  virtual void visitAttribute(ast::AstAttribute* node);
  virtual void visitAttributes(ast::AstAttribute* node);
  virtual void visitBufferField(ast::AstField* node);
  virtual void visitStateAssignment(ast::AstStateAssignment* node);
  virtual void visitSamplerState(ast::AstSamplerState* node);
  virtual void visitStructField(ast::AstField* node);
  virtual void visitSwitchCase(ast::AstSwitchCase* node);
  virtual void visitType(ast::AstType* type);
};

} // namespace visitor
