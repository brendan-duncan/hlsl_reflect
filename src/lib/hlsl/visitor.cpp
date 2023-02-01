#include "visitor.h"

namespace hlsl {

void Visitor::visitRoot(AstRoot* node) {
  AstStatement* statement = node->statement;
  while (statement != nullptr) {
    visitTopLevelStatement(statement);
    statement = statement->next;
  }
}

void Visitor::visitTopLevelStatement(AstStatement* node) {
  if (node->nodeType == AstNodeType::Struct) {
    visitStruct((AstStruct*)node);
  }
}

void Visitor::visitType(TokenType type) {
}

void Visitor::visitStatements(AstStatement* node) {
  while (node != nullptr) {
    visitStatement(node);
    node = node->next;
  }
}

void Visitor::visitStatement(AstStatement* node) {
}

void Visitor::visitDeclaration(AstDeclaration* node) {
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
}

void Visitor::visitFunction(AstFunction* node) {
}

void Visitor::visitParameter(AstParameter* node) {
}

void Visitor::visitExpressionStatement(AstExpressionStatement* node) {
}

void Visitor::visitExpression(AstExpression* node) {
}

void Visitor::visitReturn(AstReturn* node) {
}

void Visitor::visitIf(AstIf* node) {
}

void Visitor::visitFor(AstFor* node) {
}

void Visitor::visitDoWhile(AstDoWhile* node) {
}

void Visitor::visitWhile(AstWhile* node) {
}

void Visitor::visitSwitch(AstSwitch* node) {
}

void Visitor::visitSwitchCase(AstSwitchCase* node) {
}

void Visitor::visitDiscard(AstDiscard* node) {
}

} // namespace hlsl
