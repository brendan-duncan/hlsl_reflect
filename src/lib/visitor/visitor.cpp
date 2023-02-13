#include "visitor.h"

namespace visitor {

inline bool isStatement(ast::AstNode* node) {
  return node->nodeType == ast::AstNodeType::ExpressionStmt ||
         node->nodeType == ast::AstNodeType::ReturnStmt ||
         node->nodeType == ast::AstNodeType::IfStmt ||
         node->nodeType == ast::AstNodeType::ForStmt ||
         node->nodeType == ast::AstNodeType::DoWhileStmt ||
         node->nodeType == ast::AstNodeType::WhileStmt ||
         node->nodeType == ast::AstNodeType::SwitchStmt ||
         node->nodeType == ast::AstNodeType::BreakStmt ||
         node->nodeType == ast::AstNodeType::ContinueStmt ||
         node->nodeType == ast::AstNodeType::DiscardStmt ||
         node->nodeType == ast::AstNodeType::VariableStmt;
}

void Visitor::visitRoot(ast::AstRoot* node) {
  if (node == nullptr) {
    return;
  }
  ast::AstStatement* statement = node->statements;
  while (statement != nullptr) {
    visitTopLevelStatement(statement);
    statement = statement->next;
  }
}

void Visitor::visitTopLevelStatement(ast::AstStatement* node) {
  if (node->nodeType == ast::AstNodeType::StructStmt) {
    visitStructStmt((ast::AstStructStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::BufferStmt) {
    visitBufferStmt((ast::AstBufferStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::FunctionStmt) {
    visitFunctionStmt((ast::AstFunctionStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::TypedefStmt) {
    visitTypedefStmt((ast::AstTypedefStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::VariableStmt) {
    visitVariableStmt((ast::AstVariableStmt*)node);
  }
}

void Visitor::visitTypedefStmt(ast::AstTypedefStmt* node) {
  visitType(node->type);
}

void Visitor::visitType(ast::AstType* type) {
}

void Visitor::visitStatements(ast::AstStatement* node) {
  while (node != nullptr) {
    visitStatement(node);
    node = node->next;
  }
}

void Visitor::visitStatement(ast::AstStatement* node) {
  if (node->nodeType == ast::AstNodeType::ExpressionStmt) {
    visitExpressionStmt((ast::AstExpressionStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::ReturnStmt) {
    visitReturn((ast::AstReturnStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::IfStmt) {
    visitIfStmt((ast::AstIfStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::ForStmt) {
    visitForStmt((ast::AstForStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::DoWhileStmt) {
    visitDoWhileStmt((ast::AstDoWhileStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::WhileStmt) {
    visitWhileStmt((ast::AstWhileStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::SwitchStmt) {
    visitSwitchStmt((ast::AstSwitchStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::BreakStmt) {
    visitBreakStmt((ast::AstBreakStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::ContinueStmt) {
    visitContinueStmt((ast::AstContinueStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::DiscardStmt) {
    visitDiscardStmt((ast::AstDiscardStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::Block) {
    visitBlock((ast::AstBlock*)node);
  } else if (node->nodeType == ast::AstNodeType::AssignmentStmt) {
    visitAssignmentStmt((ast::AstAssignmentStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::VariableStmt) {
    visitVariableStmt((ast::AstVariableStmt*)node);
  } else if (node->nodeType == ast::AstNodeType::CallStmt) {
    visitCallStmt((ast::AstCallStmt*)node);
  }
}

void Visitor::visitBufferField(ast::AstField* node) {
  visitType(node->type);
  if (node->assignment != nullptr) {
    visitExpression(node->assignment);
  }
  if (node->next != nullptr) {
    visitBufferField(node->next);
  }
}

void Visitor::visitStructStmt(ast::AstStructStmt* node) {
  ast::AstField* field = node->fields;
  while (field != nullptr) {
    visitStructField(field);
    field = field->next;
  }
}

void Visitor::visitStructField(ast::AstField* node) {
  visitType(node->type);
}

void Visitor::visitBufferStmt(ast::AstBufferStmt* node) {
  ast::AstField* field = node->field;
  while (field != nullptr) {
    visitBufferField(field);
    field = field->next;
  }
}

void Visitor::visitFunctionStmt(ast::AstFunctionStmt* node) {
  visitType(node->returnType);
  if (node->parameters != nullptr) {
    visitParameters(node->parameters);
  }
  visitBlock(node->body);
}

void Visitor::visitParameter(ast::AstParameter* node) {
  visitType(node->type);
  if (node->initializer != nullptr) {
    visitExpression(node->initializer);
  }
}

void Visitor::visitParameters(ast::AstParameter* node) {
  visitType(node->type);
  if (node->initializer != nullptr) {
    visitExpression(node->initializer);
  }
}

void Visitor::visitExpressionStmt(ast::AstExpressionStmt* node) {
  visitExpression(node->expression);
}

void Visitor::visitReturn(ast::AstReturnStmt* node) {
  visitExpression(node->value);
}

void Visitor::visitIfStmt(ast::AstIfStmt* node) {
  visitExpression(node->condition);
  visitStatements(node->body);
  if (node->elseBody != nullptr) {
    visitStatements(node->elseBody);
  }
}

void Visitor::visitForStmt(ast::AstForStmt* node) {
  if (node->initializer != nullptr) {
    visitStatement(node->initializer);
  }
  if (node->condition != nullptr) {
    visitExpression(node->condition);
  }
  if (node->increment != nullptr) {
    visitStatements(node->increment);
  }
  visitStatements(node->body);
}

void Visitor::visitDoWhileStmt(ast::AstDoWhileStmt* node) {
  visitStatements(node->body);
  visitExpression(node->condition);
}

void Visitor::visitWhileStmt(ast::AstWhileStmt* node) {
  visitExpression(node->condition);
  visitStatements(node->body);
}

void Visitor::visitSwitchStmt(ast::AstSwitchStmt* node) {
  visitExpression(node->condition);
  ast::AstSwitchCase* switchCase = node->cases;
  while (switchCase != nullptr) {
    visitSwitchCase(switchCase);
    switchCase = switchCase->next;
  }
}

void Visitor::visitSwitchCase(ast::AstSwitchCase* node) {
  if (node->condition != nullptr) {
    visitExpression(node->condition);
  }
  if (node->body != nullptr) {
    visitStatements(node->body);
  }
}

void Visitor::visitDiscardStmt(ast::AstDiscardStmt* node) {
}

void Visitor::visitBreakStmt(ast::AstBreakStmt *node) {
}

void Visitor::visitContinueStmt(ast::AstContinueStmt *node) {
}

void Visitor::visitBlock(ast::AstBlock* node) {
  visitStatements(node->statements);
}

void Visitor::visitExpression(ast::AstExpression* node) {
  if (node == nullptr) {
    return;
  }
  if (node->nodeType == ast::AstNodeType::BinaryExpr) {
    visitBinaryExpr((ast::AstBinaryExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::PrefixExpr) {
    visitPrefixExpr((ast::AstPrefixExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::TernaryExpr) {
    visitTernaryExpr((ast::AstTernaryExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::CastExpr) {
    visitCastExpr((ast::AstCastExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::SamplerState) {
    visitSamplerState((ast::AstSamplerState*)node);
  } else if (node->nodeType == ast::AstNodeType::StringExpr) {
    visitStringExpr((ast::AstStringExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::CallExpr) {
    visitCallExpr((ast::AstCallExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::VariableExpr) {
    visitVariableExpr((ast::AstVariableExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::LiteralExpr) {
    visitLiteralExpr((ast::AstLiteralExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::StringExpr) {
    visitStringExpr((ast::AstStringExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::IncrementExpr) {
    visitIncrementExpr((ast::AstIncrementExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::ArrayExpr) {
    visitArrayExpr((ast::AstArrayExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::MemberExpr) {
    visitMemberExpr((ast::AstMemberExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::AssignmentExpr) {
    visitAssignmentExpr((ast::AstAssignmentExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::ArrayInitializerExpr) {
    visitArrayInitializerExpr((ast::AstArrayInitializerExpr*)node);
  } else if (node->nodeType == ast::AstNodeType::StructInitializerExpr) {
    visitStructInitializerExpr((ast::AstStructInitializerExpr*)node);
  }
}

void Visitor::visitArrayInitializerExpr(ast::AstArrayInitializerExpr* node) {
  ast::AstExpression* expr = node->elements;
  while (expr != nullptr) {
    visitExpression(expr);
    expr = expr->next;
  }
}

void Visitor::visitStructInitializerExpr(ast::AstStructInitializerExpr* node) {
  ast::AstExpression* expr = node->fields;
  while (expr != nullptr) {
    visitExpression(expr);
    expr = expr->next;
  }
}

void Visitor::visitBinaryExpr(ast::AstBinaryExpr *node) {
  if (node->left != nullptr) {
    visitExpression(node->left);
  }
  if (node->right != nullptr) {
    visitExpression(node->right);
  }
}

void Visitor::visitPrefixExpr(ast::AstPrefixExpr *node) {
  if (node->expression != nullptr) {
    visitExpression(node->expression);
  }
}

void Visitor::visitTernaryExpr(ast::AstTernaryExpr *node) {
  if (node->condition != nullptr) {
    visitExpression(node->condition);
  }
  if (node->trueExpr != nullptr) {
    visitExpression(node->trueExpr);
  }
  if (node->falseExpr != nullptr) {
    visitExpression(node->falseExpr);
  }
}

void Visitor::visitCastExpr(ast::AstCastExpr* node) {
  visitType(node->type);
  visitExpression(node->value);
}

void Visitor::visitArgument(ast::AstExpression* node) {
  visitExpression(node);
}

void Visitor::visitArguments(ast::AstExpression* node) {
  while (node != nullptr) {
    visitArgument(node);
    node = node->next;
  }
}

void Visitor::visitAssignmentStmt(ast::AstAssignmentStmt* node) {
  visitExpression(node->variable);
  visitExpression(node->value);
}

void Visitor::visitAttribute(ast::AstAttribute* node) {
}

void Visitor::visitAttributes(ast::AstAttribute* node) {
  while (node != nullptr) {
    visitAttribute(node);
    node = node->next;
  }
}

void Visitor::visitCallExpr(ast::AstCallExpr* node) {
  visitArguments(node->arguments);
}

void Visitor::visitLiteralExpr(ast::AstLiteralExpr* node) {

}

void Visitor::visitSamplerState(ast::AstSamplerState* node) {
  ast::AstStateAssignment* stateAssignment = node->stateAssignments;
  while (stateAssignment) {
    visitStateAssignment(stateAssignment);
    stateAssignment = stateAssignment->next;
  }
}

void Visitor::visitStateAssignment(ast::AstStateAssignment* node) {

}

void Visitor::visitStringExpr(ast::AstStringExpr* node) {

}

void Visitor::visitVariableStmt(ast::AstVariableStmt* node) {
  visitType(node->type);
  if (node->initializer != nullptr) {
    visitExpression(node->initializer);
  }
}

void Visitor::visitVariableExpr(ast::AstVariableExpr* node) {
  
}

void Visitor::visitCallStmt(ast::AstCallStmt* node) {
  visitArguments(node->arguments);
}

void Visitor::visitIncrementExpr(ast::AstIncrementExpr* node) {
  visitExpression(node->variable);
}

void Visitor::visitArrayExpr(ast::AstArrayExpr* node) {
  visitExpression(node->array);
  visitExpression(node->index);
}

void Visitor::visitMemberExpr(ast::AstMemberExpr* node) {
  visitExpression(node->object);
  visitExpression(node->member);
}

void Visitor::visitAssignmentExpr(ast::AstAssignmentExpr* node) {
  visitExpression(node->variable);
  visitExpression(node->value);
}

} // namespace visitor
