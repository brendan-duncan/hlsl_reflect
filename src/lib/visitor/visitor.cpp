#include "visitor.h"

namespace visitor {

inline bool isStatement(ast::Node* node) {
  return node->nodeType == ast::NodeType::ExpressionStmt ||
         node->nodeType == ast::NodeType::ReturnStmt ||
         node->nodeType == ast::NodeType::IfStmt ||
         node->nodeType == ast::NodeType::ForStmt ||
         node->nodeType == ast::NodeType::DoWhileStmt ||
         node->nodeType == ast::NodeType::WhileStmt ||
         node->nodeType == ast::NodeType::SwitchStmt ||
         node->nodeType == ast::NodeType::BreakStmt ||
         node->nodeType == ast::NodeType::ContinueStmt ||
         node->nodeType == ast::NodeType::DiscardStmt ||
         node->nodeType == ast::NodeType::VariableStmt;
}

void Visitor::visitRoot(ast::Root* node) {
  if (node == nullptr) {
    return;
  }
  ast::Statement* statement = node->statements;
  while (statement != nullptr) {
    visitTopLevelStatement(statement);
    statement = statement->next;
  }
}

void Visitor::visitTopLevelStatement(ast::Statement* node) {
  if (node->visible == false) {
    return;
  }
  if (node->nodeType == ast::NodeType::StructStmt) {
    visitStructStmt((ast::StructStmt*)node);
  } else if (node->nodeType == ast::NodeType::BufferStmt) {
    visitBufferStmt((ast::BufferStmt*)node);
  } else if (node->nodeType == ast::NodeType::FunctionStmt) {
    visitFunctionStmt((ast::FunctionStmt*)node);
  } else if (node->nodeType == ast::NodeType::TypedefStmt) {
    visitTypedefStmt((ast::TypedefStmt*)node);
  } else if (node->nodeType == ast::NodeType::VariableStmt) {
    visitVariableStmt((ast::VariableStmt*)node);
  }
}

void Visitor::visitTypedefStmt(ast::TypedefStmt* node) {
  visitType(node->type);
}

void Visitor::visitType(ast::Type* type) {
}

void Visitor::visitStatements(ast::Statement* node) {
  while (node != nullptr) {
    visitStatement(node);
    node = node->next;
  }
}

void Visitor::visitStatement(ast::Statement* node) {
  if (node->nodeType == ast::NodeType::ExpressionStmt) {
    visitExpressionStmt((ast::ExpressionStmt*)node);
  } else if (node->nodeType == ast::NodeType::ReturnStmt) {
    visitReturn((ast::ReturnStmt*)node);
  } else if (node->nodeType == ast::NodeType::IfStmt) {
    visitIfStmt((ast::IfStmt*)node);
  } else if (node->nodeType == ast::NodeType::ForStmt) {
    visitForStmt((ast::ForStmt*)node);
  } else if (node->nodeType == ast::NodeType::DoWhileStmt) {
    visitDoWhileStmt((ast::DoWhileStmt*)node);
  } else if (node->nodeType == ast::NodeType::WhileStmt) {
    visitWhileStmt((ast::WhileStmt*)node);
  } else if (node->nodeType == ast::NodeType::SwitchStmt) {
    visitSwitchStmt((ast::SwitchStmt*)node);
  } else if (node->nodeType == ast::NodeType::BreakStmt) {
    visitBreakStmt((ast::BreakStmt*)node);
  } else if (node->nodeType == ast::NodeType::ContinueStmt) {
    visitContinueStmt((ast::ContinueStmt*)node);
  } else if (node->nodeType == ast::NodeType::DiscardStmt) {
    visitDiscardStmt((ast::DiscardStmt*)node);
  } else if (node->nodeType == ast::NodeType::Block) {
    visitBlock((ast::Block*)node);
  } else if (node->nodeType == ast::NodeType::AssignmentStmt) {
    visitAssignmentStmt((ast::AssignmentStmt*)node);
  } else if (node->nodeType == ast::NodeType::VariableStmt) {
    visitVariableStmt((ast::VariableStmt*)node);
  } else if (node->nodeType == ast::NodeType::CallStmt) {
    visitCallStmt((ast::CallStmt*)node);
  }
}

void Visitor::visitBufferField(ast::Field* node) {
  visitType(node->type);
  if (node->assignment != nullptr) {
    visitExpression(node->assignment);
  }
  if (node->next != nullptr) {
    visitBufferField(node->next);
  }
}

void Visitor::visitStructStmt(ast::StructStmt* node) {
  ast::Field* field = node->fields;
  while (field != nullptr) {
    visitStructField(field);
    field = field->next;
  }
}

void Visitor::visitStructField(ast::Field* node) {
  visitType(node->type);
}

void Visitor::visitBufferStmt(ast::BufferStmt* node) {
  ast::Field* field = node->field;
  while (field != nullptr) {
    visitBufferField(field);
    field = field->next;
  }
}

void Visitor::visitFunctionStmt(ast::FunctionStmt* node) {
  visitType(node->returnType);
  if (node->parameters != nullptr) {
    visitParameter(node->parameters);
  }
  visitBlock(node->body);
}

void Visitor::visitParameter(ast::Parameter* node) {
  visitType(node->type);
  if (node->initializer != nullptr) {
    visitExpression(node->initializer);
  }
}

void Visitor::visitParameters(ast::Parameter* node) {
  ast::Parameter* p = node;
  while (p != nullptr) {
    visitParameter(p);
    p = p->next;
  }
}

void Visitor::visitExpressionStmt(ast::ExpressionStmt* node) {
  visitExpression(node->expression);
}

void Visitor::visitReturn(ast::ReturnStmt* node) {
  visitExpression(node->value);
}

void Visitor::visitIfStmt(ast::IfStmt* node) {
  visitExpression(node->condition);
  visitStatements(node->body);
  if (node->elseBody != nullptr) {
    visitStatements(node->elseBody);
  }
}

void Visitor::visitForStmt(ast::ForStmt* node) {
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

void Visitor::visitDoWhileStmt(ast::DoWhileStmt* node) {
  visitStatements(node->body);
  visitExpression(node->condition);
}

void Visitor::visitWhileStmt(ast::WhileStmt* node) {
  visitExpression(node->condition);
  visitStatements(node->body);
}

void Visitor::visitSwitchStmt(ast::SwitchStmt* node) {
  visitExpression(node->condition);
  ast::SwitchCase* switchCase = node->cases;
  while (switchCase != nullptr) {
    visitSwitchCase(switchCase);
    switchCase = switchCase->next;
  }
}

void Visitor::visitSwitchCase(ast::SwitchCase* node) {
  if (node->condition != nullptr) {
    visitExpression(node->condition);
  }
  if (node->body != nullptr) {
    visitStatements(node->body);
  }
}

void Visitor::visitDiscardStmt(ast::DiscardStmt* node) {
}

void Visitor::visitBreakStmt(ast::BreakStmt *node) {
}

void Visitor::visitContinueStmt(ast::ContinueStmt *node) {
}

void Visitor::visitBlock(ast::Block* node) {
  visitStatements(node->statements);
}

void Visitor::visitExpression(ast::Expression* node) {
  if (node == nullptr) {
    return;
  }
  if (node->nodeType == ast::NodeType::BinaryExpr) {
    visitBinaryExpr((ast::BinaryExpr*)node);
  } else if (node->nodeType == ast::NodeType::PrefixExpr) {
    visitPrefixExpr((ast::PrefixExpr*)node);
  } else if (node->nodeType == ast::NodeType::TernaryExpr) {
    visitTernaryExpr((ast::TernaryExpr*)node);
  } else if (node->nodeType == ast::NodeType::CastExpr) {
    visitCastExpr((ast::CastExpr*)node);
  } else if (node->nodeType == ast::NodeType::SamplerState) {
    visitSamplerState((ast::SamplerState*)node);
  } else if (node->nodeType == ast::NodeType::StringExpr) {
    visitStringExpr((ast::StringExpr*)node);
  } else if (node->nodeType == ast::NodeType::CallExpr) {
    visitCallExpr((ast::CallExpr*)node);
  } else if (node->nodeType == ast::NodeType::VariableExpr) {
    visitVariableExpr((ast::VariableExpr*)node);
  } else if (node->nodeType == ast::NodeType::LiteralExpr) {
    visitLiteralExpr((ast::LiteralExpr*)node);
  } else if (node->nodeType == ast::NodeType::StringExpr) {
    visitStringExpr((ast::StringExpr*)node);
  } else if (node->nodeType == ast::NodeType::IncrementExpr) {
    visitIncrementExpr((ast::IncrementExpr*)node);
  } else if (node->nodeType == ast::NodeType::ArrayExpr) {
    visitArrayExpr((ast::ArrayExpr*)node);
  } else if (node->nodeType == ast::NodeType::MemberExpr) {
    visitMemberExpr((ast::MemberExpr*)node);
  } else if (node->nodeType == ast::NodeType::AssignmentExpr) {
    visitAssignmentExpr((ast::AssignmentExpr*)node);
  } else if (node->nodeType == ast::NodeType::ArrayInitializerExpr) {
    visitArrayInitializerExpr((ast::ArrayInitializerExpr*)node);
  } else if (node->nodeType == ast::NodeType::StructInitializerExpr) {
    visitStructInitializerExpr((ast::StructInitializerExpr*)node);
  }
}

void Visitor::visitArrayInitializerExpr(ast::ArrayInitializerExpr* node) {
  ast::Expression* expr = node->elements;
  while (expr != nullptr) {
    visitExpression(expr);
    expr = expr->next;
  }
}

void Visitor::visitStructInitializerExpr(ast::StructInitializerExpr* node) {
  ast::Expression* expr = node->fields;
  while (expr != nullptr) {
    visitExpression(expr);
    expr = expr->next;
  }
}

void Visitor::visitBinaryExpr(ast::BinaryExpr *node) {
  if (node->left != nullptr) {
    visitExpression(node->left);
  }
  if (node->right != nullptr) {
    visitExpression(node->right);
  }
}

void Visitor::visitPrefixExpr(ast::PrefixExpr *node) {
  if (node->expression != nullptr) {
    visitExpression(node->expression);
  }
}

void Visitor::visitTernaryExpr(ast::TernaryExpr *node) {
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

void Visitor::visitCastExpr(ast::CastExpr* node) {
  visitType(node->type);
  visitExpression(node->value);
}

void Visitor::visitArgument(ast::Expression* node) {
  visitExpression(node);
}

void Visitor::visitArguments(ast::Expression* node) {
  while (node != nullptr) {
    visitArgument(node);
    node = node->next;
  }
}

void Visitor::visitAssignmentStmt(ast::AssignmentStmt* node) {
  visitExpression(node->variable);
  visitExpression(node->value);
}

void Visitor::visitAttribute(ast::Attribute* node) {
}

void Visitor::visitAttributes(ast::Attribute* node) {
  while (node != nullptr) {
    visitAttribute(node);
    node = node->next;
  }
}

void Visitor::visitCallExpr(ast::CallExpr* node) {
  visitArguments(node->arguments);
}

void Visitor::visitLiteralExpr(ast::LiteralExpr* node) {

}

void Visitor::visitSamplerState(ast::SamplerState* node) {
  ast::StateAssignment* stateAssignment = node->stateAssignments;
  while (stateAssignment) {
    visitStateAssignment(stateAssignment);
    stateAssignment = stateAssignment->next;
  }
}

void Visitor::visitStateAssignment(ast::StateAssignment* node) {

}

void Visitor::visitStringExpr(ast::StringExpr* node) {

}

void Visitor::visitVariableStmt(ast::VariableStmt* node) {
  visitType(node->type);
  if (node->initializer != nullptr) {
    visitExpression(node->initializer);
  }
}

void Visitor::visitVariableExpr(ast::VariableExpr* node) {
  
}

void Visitor::visitCallStmt(ast::CallStmt* node) {
  visitArguments(node->arguments);
}

void Visitor::visitIncrementExpr(ast::IncrementExpr* node) {
  visitExpression(node->variable);
}

void Visitor::visitArrayExpr(ast::ArrayExpr* node) {
  visitExpression(node->array);
  visitExpression(node->index);
}

void Visitor::visitMemberExpr(ast::MemberExpr* node) {
  visitExpression(node->object);
  visitExpression(node->member);
}

void Visitor::visitAssignmentExpr(ast::AssignmentExpr* node) {
  visitExpression(node->variable);
  visitExpression(node->value);
}

} // namespace visitor
