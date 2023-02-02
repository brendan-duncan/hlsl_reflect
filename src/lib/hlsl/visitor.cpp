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
         node->nodeType == AstNodeType::DiscardStmt;
}

void Visitor::visitRoot(AstRoot* node) {
  AstStatement* statement = node->statements;
  while (statement != nullptr) {
    visitTopLevelStatement(statement);
    statement = statement->next;
  }
}

void Visitor::visitTopLevelStatement(AstStatement* node) {
  if (node->nodeType == AstNodeType::StructStmt) {
    visitStruct((AstStructStmt*)node);
  } else if (node->nodeType == AstNodeType::BufferStmt) {
    visitBuffer((AstBufferStmt*)node);
  } else if (node->nodeType == AstNodeType::FunctionStmt) {
    visitFunction((AstFunctionStmt*)node);
  } else if (isStatement(node)) {
    visitStatement((AstStatement*)node);
  }
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
    visitExpressionStatement((AstExpressionStmt*)node);
  } else if (node->nodeType == AstNodeType::ReturnStmt) {
    visitReturn((AstReturnStmt*)node);
  } else if (node->nodeType == AstNodeType::IfStmt) {
    visitIf((AstIfStmt*)node);
  } else if (node->nodeType == AstNodeType::ForStmt) {
    visitFor((AstForStmt*)node);
  } else if (node->nodeType == AstNodeType::DoWhileStmt) {
    visitDoWhile((AstDoWhileStmt*)node);
  } else if (node->nodeType == AstNodeType::WhileStmt) {
    visitWhile((AstWhileStmt*)node);
  } else if (node->nodeType == AstNodeType::SwitchStmt) {
    visitSwitch((AstSwitchStmt*)node);
  } else if (node->nodeType == AstNodeType::BreakStmt) {
    visitBreak((AstBreakStmt*)node);
  } else if (node->nodeType == AstNodeType::ContinueStmt) {
    visitContinue((AstContinueStmt*)node);
  } else if (node->nodeType == AstNodeType::DiscardStmt) {
    visitDiscard((AstDiscardStmt*)node);
  } else if (node->nodeType == AstNodeType::Block) {
    visitBlock((AstBlock*)node);
  } else if (node->nodeType == AstNodeType::AssignmentStmt) {
    visitAssignment((AstAssignmentStmt*)node);
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

void Visitor::visitStruct(AstStructStmt* node) {
  AstStructField* field = node->field;
  while (field != nullptr) {
    visitStructField(field);
    field = field->next;
  }
}

void Visitor::visitStructField(AstStructField* node) {
  visitType(node->type);
}

void Visitor::visitBuffer(AstBufferStmt* node) {
  AstBufferField* field = node->field;
  while (field != nullptr) {
    visitBufferField(field);
    field = field->next;
  }
}

void Visitor::visitFunction(AstFunctionStmt* node) {
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

void Visitor::visitExpressionStatement(AstExpressionStmt* node) {
  visitExpression(node->expression);
}

void Visitor::visitReturn(AstReturnStmt* node) {
  visitExpression(node->value);
}

void Visitor::visitIf(AstIfStmt* node) {
  visitExpression(node->condition);
  visitStatements(node->body);
  if (node->elseBody != nullptr) {
    visitStatements(node->elseBody);
  }
}

void Visitor::visitFor(AstForStmt* node) {
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

void Visitor::visitDoWhile(AstDoWhileStmt* node) {
  visitStatements(node->body);
  visitExpression(node->condition);
}

void Visitor::visitWhile(AstWhileStmt* node) {
  visitExpression(node->condition);
  visitStatements(node->body);
}

void Visitor::visitSwitch(AstSwitchStmt* node) {
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

void Visitor::visitDiscard(AstDiscardStmt* node) {
}

void Visitor::visitBreak(AstBreakStmt *node) {
}

void Visitor::visitContinue(AstContinueStmt *node) {
}

void Visitor::visitBlock(AstBlock* node) {
  visitStatements(node->statements);
}

void Visitor::visitExpression(AstExpression* node) {
  if (node->nodeType == AstNodeType::BinaryExpr) {
    visitBinaryOperator((AstBinaryExpr*)node);
  } else if (node->nodeType == AstNodeType::UnaryExpr) {
    visitUnaryOperator((AstUnaryExpr*)node);
  } else if (node->nodeType == AstNodeType::TernaryExpr) {
    visitTernaryOperator((AstTernaryExpr*)node);
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
  }
}

void Visitor::visitBinaryOperator(AstBinaryExpr *node) {
  if (node->left != nullptr) {
    visitExpression(node->left);
  }
  if (node->right != nullptr) {
    visitExpression(node->right);
  }
}

void Visitor::visitUnaryOperator(AstUnaryExpr *node) {
  if (node->expression != nullptr) {
    visitExpression(node->expression);
  }
}

void Visitor::visitTernaryOperator(AstTernaryExpr *node) {
  if (node->condition != nullptr) {
    visitExpression(node->condition);
  }
  if (node->trueExpression != nullptr) {
    visitExpression(node->trueExpression);
  }
  if (node->falseExpression != nullptr) {
    visitExpression(node->falseExpression);
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

void Visitor::visitAssignment(AstAssignmentStmt* node) {
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

void Visitor::visitVariable(AstVariableStmt* node) {
  visitType(node->type);
  if (node->initializer != nullptr) {
    visitExpression(node->initializer);
  }
}

void Visitor::visitVariableExpr(AstVariableExpr* node) {
  
}

void Visitor::visitCall(AstCallStmt* node) {
  visitArguments(node->arguments);
}

} // namespace hlsl
