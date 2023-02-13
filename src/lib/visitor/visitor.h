#include "../ast/ast_node.h"

namespace visitor {

class Visitor {
public:
  virtual void visitRoot(ast::Root* node);

  virtual void visitTopLevelStatement(ast::Statement* node);
  virtual void visitStatements(ast::Statement* node);
  
  virtual void visitStatement(ast::Statement* node);
  virtual void visitBlock(ast::Block* node);
  virtual void visitBreakStmt(ast::BreakStmt* node);
  virtual void visitBufferStmt(ast::BufferStmt* node);
  virtual void visitContinueStmt(ast::ContinueStmt* node);
  virtual void visitDiscardStmt(ast::DiscardStmt* node);
  virtual void visitDoWhileStmt(ast::DoWhileStmt* node);
  virtual void visitExpressionStmt(ast::ExpressionStmt* node);
  virtual void visitForStmt(ast::ForStmt* node);
  virtual void visitFunctionStmt(ast::FunctionStmt* node);
  virtual void visitIfStmt(ast::IfStmt* node);
  virtual void visitStructStmt(ast::StructStmt* node);
  virtual void visitSwitchStmt(ast::SwitchStmt* node);
  virtual void visitVariableStmt(ast::VariableStmt* node);
  virtual void visitTypedefStmt(ast::TypedefStmt* node);
  virtual void visitWhileStmt(ast::WhileStmt* node);
  virtual void visitCallStmt(ast::CallStmt* node);
  virtual void visitAssignmentStmt(ast::AssignmentStmt* node);

  virtual void visitExpression(ast::Expression* node);  
  virtual void visitBinaryExpr(ast::BinaryExpr* node);
  virtual void visitCallExpr(ast::CallExpr* node);
  virtual void visitLiteralExpr(ast::LiteralExpr* node);
  virtual void visitCastExpr(ast::CastExpr* node);
  virtual void visitParameter(ast::Parameter* node);
  virtual void visitParameters(ast::Parameter* node);
  virtual void visitReturn(ast::ReturnStmt* node); 
  virtual void visitStringExpr(ast::StringExpr* node);
  virtual void visitTernaryExpr(ast::TernaryExpr* node);
  virtual void visitPrefixExpr(ast::PrefixExpr* node);  
  virtual void visitVariableExpr(ast::VariableExpr* node);
  virtual void visitIncrementExpr(ast::IncrementExpr* node);
  virtual void visitArrayExpr(ast::ArrayExpr* node);
  virtual void visitMemberExpr(ast::MemberExpr* node);
  virtual void visitAssignmentExpr(ast::AssignmentExpr* node);
  virtual void visitArrayInitializerExpr(ast::ArrayInitializerExpr* node);
  virtual void visitStructInitializerExpr(ast::StructInitializerExpr* node);

  virtual void visitArgument(ast::Expression* node);
  virtual void visitArguments(ast::Expression* node);
  virtual void visitAttribute(ast::Attribute* node);
  virtual void visitAttributes(ast::Attribute* node);
  virtual void visitBufferField(ast::Field* node);
  virtual void visitStateAssignment(ast::StateAssignment* node);
  virtual void visitSamplerState(ast::SamplerState* node);
  virtual void visitStructField(ast::Field* node);
  virtual void visitSwitchCase(ast::SwitchCase* node);
  virtual void visitType(ast::Type* type);
};

} // namespace visitor
