#include "visitor.h"

namespace hlsl {

inline bool isStatement(AstNode* node) {
  return node->nodeType == AstNodeType::ExpressionStatement ||
         node->nodeType == AstNodeType::Return ||
         node->nodeType == AstNodeType::If ||
         node->nodeType == AstNodeType::For ||
         node->nodeType == AstNodeType::DoWhile ||
         node->nodeType == AstNodeType::While ||
         node->nodeType == AstNodeType::Switch ||
         node->nodeType == AstNodeType::Break ||
         node->nodeType == AstNodeType::Continue ||
         node->nodeType == AstNodeType::Discard;
}

void Visitor::visitRoot(AstRoot* node) {
  AstStatement* statement = node->statements;
  while (statement != nullptr) {
    visitTopLevelStatement(statement);
    statement = statement->next;
  }
}

void Visitor::visitTopLevelStatement(AstStatement* node) {
  if (node->nodeType == AstNodeType::Struct) {
    visitStruct((AstStruct*)node);
  } else if (node->nodeType == AstNodeType::Buffer) {
    visitBuffer((AstBuffer*)node);
  } else if (node->nodeType == AstNodeType::Function) {
    visitFunction((AstFunction*)node);
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
  if (node->nodeType == AstNodeType::ExpressionStatement) {
    visitExpressionStatement((AstExpressionStatement*)node);
  } else if (node->nodeType == AstNodeType::Return) {
    visitReturn((AstReturn*)node);
  } else if (node->nodeType == AstNodeType::If) {
    visitIf((AstIf*)node);
  } else if (node->nodeType == AstNodeType::For) {
    visitFor((AstFor*)node);
  } else if (node->nodeType == AstNodeType::DoWhile) {
    visitDoWhile((AstDoWhile*)node);
  } else if (node->nodeType == AstNodeType::While) {
    visitWhile((AstWhile*)node);
  } else if (node->nodeType == AstNodeType::Switch) {
    visitSwitch((AstSwitch*)node);
  } else if (node->nodeType == AstNodeType::Break) {
    visitBreak((AstBreak*)node);
  } else if (node->nodeType == AstNodeType::Continue) {
    visitContinue((AstContinue*)node);
  } else if (node->nodeType == AstNodeType::Discard) {
    visitDiscard((AstDiscard*)node);
  } else if (node->nodeType == AstNodeType::Block) {
    visitBlock((AstBlock*)node);
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

void Visitor::visitStruct(AstStruct* node) {
  AstStructField* field = node->field;
  while (field != nullptr) {
    visitStructField(field);
    field = field->next;
  }
}

void Visitor::visitStructField(AstStructField* node) {
  visitType(node->type);
}

void Visitor::visitBuffer(AstBuffer* node) {
  AstBufferField* field = node->field;
  while (field != nullptr) {
    visitBufferField(field);
    field = field->next;
  }
}

void Visitor::visitFunction(AstFunction* node) {
  visitType(node->returnType);
  AstParameter* parameter = node->parameters;
  while (parameter != nullptr) {
    visitParameter(parameter);
    parameter = (AstParameter*)parameter->next;
  }
  visitStatements(node->body);
}

void Visitor::visitParameter(AstParameter* node) {
  visitType(node->type);
  if (node->initializer != nullptr) {
    visitExpression(node->initializer);
  }
}

void Visitor::visitExpressionStatement(AstExpressionStatement* node) {
  visitExpression(node->expression);
}

void Visitor::visitReturn(AstReturn* node) {
  visitExpression(node->value);
}

void Visitor::visitIf(AstIf* node) {
  visitExpression(node->condition);
  visitStatements(node->body);
  if (node->elseBody != nullptr) {
    visitStatements(node->elseBody);
  }
}

void Visitor::visitFor(AstFor* node) {
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

void Visitor::visitDoWhile(AstDoWhile* node) {
  visitStatements(node->body);
  visitExpression(node->condition);
}

void Visitor::visitWhile(AstWhile* node) {
  visitExpression(node->condition);
  visitStatements(node->body);
}

void Visitor::visitSwitch(AstSwitch* node) {
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

void Visitor::visitDiscard(AstDiscard* node) {
}

void Visitor::visitBreak(AstBreak *node) {
}

void Visitor::visitContinue(AstContinue *node) {
}

void Visitor::visitBlock(AstBlock* node) {
  visitStatements(node->statements);
}

void Visitor::visitExpression(AstExpression* node) {
  if (node->astType == AstNodeType::BinaryOperator) {
    visitBinaryOperator((AstBinaryOperator*)node);
  } else if (node->astType == AstNodeType::UnaryOperator) {
    visitUnaryOperator((AstUnaryOperator*)node);
  } else if (node->astType == AstNodeType::TernaryOperator) {
    visitTernaryOperator((AstTernaryOperator*)node);
  } else if (node->astType == AstNodeType::CastExpr) {
    visitCastExpr((AstCastExpr*)node);
  }
}

void Visitor::visitBinaryOperator(AstBinaryOperator *node) {

}

void Visitor::visitUnaryOperator(AstUnaryOperator *node) {

}

void Visitor::visitTernaryOperator(AstTernaryOperator *node) {

}

void Visitor::visitCastExpr(AstCastExpr* node) {
  visitType(node->type);
  visitExpression(node->expression);
}

void Visitor::visitArgument(AstArgument* node) {

}

void Visitor::visitAssignment(AstAssignment* node) {

}

void Visitor::visitAttribute(AstAttribute* node) {

}

void Visitor::visitCallExpr(AstCallExpr* node) {

}

void Visitor::visitLiteralExpr(AstLiteralExpr* node) {

}

void Visitor::visitNode(AstNode* node) {

}

void Visitor::visitSamplerState(AstSamplerState* node) {

}

void Visitor::visitStateAssignment(AstStateAssignment* node) {

}

void Visitor::visitStringExpr(AstStringExpr* node) {

}

void Visitor::visitVariable(AstVariable* node) {

}

void Visitor::visitVariableExpr(AstVariableExpr* node) {

}

void Visitor::visitCall(AstCall* node) {

}

} // namespace hlsl
