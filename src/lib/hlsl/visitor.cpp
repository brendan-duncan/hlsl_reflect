#include "visitor.h"

namespace hlsl {

inline bool isStatement(AstNode* node) {
  return node->nodeType == AstNodeType::ExpressionStmt ||
         node->nodeType == AstNodeType::ReturnStmt ||
         node->nodeType == AstNodeType::IfStmt ||
         node->nodeType == AstNodeType::ForStmt ||
         node->nodeType == AstNodeType::DoWhileStmt ||
         node->nodeType == AstNodeType::WhileStmt ||
         node->nodeType == AstNodeType::SwitchStmt ||
         node->nodeType == AstNodeType::BreakStmt ||
         node->nodeType == AstNodeType::ContinueStmt ||
         node->nodeType == AstNodeType::DiscardStmt ||
         node->nodeType == AstNodeType::VariableStmt;
}

void Visitor::visitRoot(AstRoot* node) {
  if (node == nullptr) {
    return;
  }
  AstStatement* statement = node->statements;
  while (statement != nullptr) {
    visitTopLevelStatement(statement);
    statement = statement->next;
  }
}

void Visitor::visitTopLevelStatement(AstStatement* node) {
  if (node->nodeType == AstNodeType::StructStmt) {
    visitStructStmt((AstStructStmt*)node);
  } else if (node->nodeType == AstNodeType::BufferStmt) {
    visitBufferStmt((AstBufferStmt*)node);
  } else if (node->nodeType == AstNodeType::FunctionStmt) {
    visitFunctionStmt((AstFunctionStmt*)node);
  } else if (node->nodeType == AstNodeType::TypedefStmt) {
    visitTypedefStmt((AstTypedefStmt*)node);
  } else if (node->nodeType == AstNodeType::VariableStmt) {
    visitVariableStmt((AstVariableStmt*)node);
    std::cout << ";" << std::endl;
  }
}

void Visitor::visitTypedefStmt(AstTypedefStmt* node) {
  visitType(node->type);
}

void Visitor::visitType(AstType* type) {
}

void Visitor::visitStatements(AstStatement* node) {
  while (node != nullptr) {
    visitStatement(node);
    node = node->next;
  }
}

void Visitor::visitStatement(AstStatement* node) {
  if (node->nodeType == AstNodeType::ExpressionStmt) {
    visitExpressionStmt((AstExpressionStmt*)node);
  } else if (node->nodeType == AstNodeType::ReturnStmt) {
    visitReturn((AstReturnStmt*)node);
  } else if (node->nodeType == AstNodeType::IfStmt) {
    visitIfStmt((AstIfStmt*)node);
  } else if (node->nodeType == AstNodeType::ForStmt) {
    visitForStmt((AstForStmt*)node);
  } else if (node->nodeType == AstNodeType::DoWhileStmt) {
    visitDoWhileStmt((AstDoWhileStmt*)node);
  } else if (node->nodeType == AstNodeType::WhileStmt) {
    visitWhileStmt((AstWhileStmt*)node);
  } else if (node->nodeType == AstNodeType::SwitchStmt) {
    visitSwitchStmt((AstSwitchStmt*)node);
  } else if (node->nodeType == AstNodeType::BreakStmt) {
    visitBreakStmt((AstBreakStmt*)node);
  } else if (node->nodeType == AstNodeType::ContinueStmt) {
    visitContinueStmt((AstContinueStmt*)node);
  } else if (node->nodeType == AstNodeType::DiscardStmt) {
    visitDiscardStmt((AstDiscardStmt*)node);
  } else if (node->nodeType == AstNodeType::Block) {
    visitBlock((AstBlock*)node);
  } else if (node->nodeType == AstNodeType::AssignmentStmt) {
    visitAssignmentStmt((AstAssignmentStmt*)node);
  } else if (node->nodeType == AstNodeType::VariableStmt) {
    visitVariableStmt((AstVariableStmt*)node);
  } else if (node->nodeType == AstNodeType::CallStmt) {
    visitCallStmt((AstCallStmt*)node);
  }
}

void Visitor::visitBufferField(AstBufferField* node) {
  visitType(node->type);
  if (node->assignment != nullptr) {
    visitExpression(node->assignment);
  }
  if (node->next != nullptr) {
    visitBufferField(node->next);
  }
}

void Visitor::visitStructStmt(AstStructStmt* node) {
  AstStructField* field = node->fields;
  while (field != nullptr) {
    visitStructField(field);
    field = field->next;
  }
}

void Visitor::visitStructField(AstStructField* node) {
  visitType(node->type);
}

void Visitor::visitBufferStmt(AstBufferStmt* node) {
  AstBufferField* field = node->field;
  while (field != nullptr) {
    visitBufferField(field);
    field = field->next;
  }
}

void Visitor::visitFunctionStmt(AstFunctionStmt* node) {
  visitType(node->returnType);
  visitParameters(node->parameters);
  visitBlock(node->body);
}

void Visitor::visitParameter(AstParameter* node) {
  visitType(node->type);
  if (node->initializer != nullptr) {
    visitExpression(node->initializer);
  }
}

void Visitor::visitParameters(AstParameter* node) {
  visitType(node->type);
  if (node->initializer != nullptr) {
    visitExpression(node->initializer);
  }
}

void Visitor::visitExpressionStmt(AstExpressionStmt* node) {
  visitExpression(node->expression);
}

void Visitor::visitReturn(AstReturnStmt* node) {
  visitExpression(node->value);
}

void Visitor::visitIfStmt(AstIfStmt* node) {
  visitExpression(node->condition);
  visitStatements(node->body);
  if (node->elseBody != nullptr) {
    visitStatements(node->elseBody);
  }
}

void Visitor::visitForStmt(AstForStmt* node) {
  if (node->initializer != nullptr) {
    visitStatement(node->initializer);
  }
  if (node->condition != nullptr) {
    visitExpression(node->condition);
  }
  if (node->increment != nullptr) {
    visitExpression(node->increment);
  }
  visitStatements(node->body);
}

void Visitor::visitDoWhileStmt(AstDoWhileStmt* node) {
  visitStatements(node->body);
  visitExpression(node->condition);
}

void Visitor::visitWhileStmt(AstWhileStmt* node) {
  visitExpression(node->condition);
  visitStatements(node->body);
}

void Visitor::visitSwitchStmt(AstSwitchStmt* node) {
  visitExpression(node->condition);
  AstSwitchCase* switchCase = node->cases;
  while (switchCase != nullptr) {
    visitSwitchCase(switchCase);
    switchCase = switchCase->next;
  }
}

void Visitor::visitSwitchCase(AstSwitchCase* node) {
  if (node->condition != nullptr) {
    visitExpression(node->condition);
  }
  if (node->body != nullptr) {
    visitStatements(node->body);
  }
}

void Visitor::visitDiscardStmt(AstDiscardStmt* node) {
}

void Visitor::visitBreakStmt(AstBreakStmt *node) {
}

void Visitor::visitContinueStmt(AstContinueStmt *node) {
}

void Visitor::visitBlock(AstBlock* node) {
  visitStatements(node->statements);
}

void Visitor::visitExpression(AstExpression* node) {
  if (node == nullptr) {
    return;
  }
  if (node->nodeType == AstNodeType::BinaryExpr) {
    visitBinaryExpr((AstBinaryExpr*)node);
  } else if (node->nodeType == AstNodeType::PrefixExpr) {
    visitPrefixExpr((AstPrefixExpr*)node);
  } else if (node->nodeType == AstNodeType::TernaryExpr) {
    visitTernaryExpr((AstTernaryExpr*)node);
  } else if (node->nodeType == AstNodeType::CastExpr) {
    visitCastExpr((AstCastExpr*)node);
  } else if (node->nodeType == AstNodeType::SamplerState) {
    visitSamplerState((AstSamplerState*)node);
  } else if (node->nodeType == AstNodeType::StringExpr) {
    visitStringExpr((AstStringExpr*)node);
  } else if (node->nodeType == AstNodeType::CallExpr) {
    visitCallExpr((AstCallExpr*)node);
  } else if (node->nodeType == AstNodeType::VariableExpr) {
    visitVariableExpr((AstVariableExpr*)node);
  } else if (node->nodeType == AstNodeType::LiteralExpr) {
    visitLiteralExpr((AstLiteralExpr*)node);
  } else if (node->nodeType == AstNodeType::StringExpr) {
    visitStringExpr((AstStringExpr*)node);
  } else if (node->nodeType == AstNodeType::IncrementExpr) {
    visitIncrementExpr((AstIncrementExpr*)node);
  } else if (node->nodeType == AstNodeType::ArrayExpr) {
    visitArrayExpr((AstArrayExpr*)node);
  } else if (node->nodeType == AstNodeType::MemberExpr) {
    visitMemberExpr((AstMemberExpr*)node);
  } else if (node->nodeType == AstNodeType::AssignmentExpr) {
    visitAssignmentExpr((AstAssignmentExpr*)node);
  }
}

void Visitor::visitBinaryExpr(AstBinaryExpr *node) {
  if (node->left != nullptr) {
    visitExpression(node->left);
  }
  if (node->right != nullptr) {
    visitExpression(node->right);
  }
}

void Visitor::visitPrefixExpr(AstPrefixExpr *node) {
  if (node->expression != nullptr) {
    visitExpression(node->expression);
  }
}

void Visitor::visitTernaryExpr(AstTernaryExpr *node) {
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

void Visitor::visitCastExpr(AstCastExpr* node) {
  visitType(node->type);
  visitExpression(node->expression);
}

void Visitor::visitArgument(AstExpression* node) {
  visitExpression(node);
}

void Visitor::visitArguments(AstExpression* node) {
  while (node != nullptr) {
    visitArgument(node);
    node = node->next;
  }
}

void Visitor::visitAssignmentStmt(AstAssignmentStmt* node) {
  visitExpression(node->variable);
  visitExpression(node->value);
}

void Visitor::visitAttribute(AstAttribute* node) {
}

void Visitor::visitAttributes(AstAttribute* node) {
  while (node != nullptr) {
    visitAttribute(node);
    node = node->next;
  }
}

void Visitor::visitCallExpr(AstCallExpr* node) {
  visitArguments(node->arguments);
}

void Visitor::visitLiteralExpr(AstLiteralExpr* node) {

}

void Visitor::visitSamplerState(AstSamplerState* node) {
  AstStateAssignment* stateAssignment = node->stateAssignments;
  while (stateAssignment) {
    visitStateAssignment(stateAssignment);
    stateAssignment = stateAssignment->next;
  }
}

void Visitor::visitStateAssignment(AstStateAssignment* node) {

}

void Visitor::visitStringExpr(AstStringExpr* node) {

}

void Visitor::visitVariableStmt(AstVariableStmt* node) {
  visitType(node->type);
  if (node->initializer != nullptr) {
    visitExpression(node->initializer);
  }
}

void Visitor::visitVariableExpr(AstVariableExpr* node) {
  
}

void Visitor::visitCallStmt(AstCallStmt* node) {
  visitArguments(node->arguments);
}

void Visitor::visitIncrementExpr(AstIncrementExpr* node) {
  visitExpression(node->variable);
}

void Visitor::visitArrayExpr(AstArrayExpr* node) {
  visitExpression(node->array);
  visitExpression(node->index);
}

void Visitor::visitMemberExpr(AstMemberExpr* node) {
  visitExpression(node->object);
  visitExpression(node->member);
}

void Visitor::visitAssignmentExpr(AstAssignmentExpr* node) {
  visitExpression(node->variable);
  visitExpression(node->value);
}

} // namespace hlsl
