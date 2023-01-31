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

void Visitor::visitStatements(AstStatement* node) {
  while (node != nullptr) {
    visitStatement(node);
    node = node->next;
  }
}

void Visitor::visitStatement(AstStatement* node) {
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

} // namespace hlsl
