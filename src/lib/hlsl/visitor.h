#include "ast/ast_node.h"

namespace hlsl {

class Visitor {
public:
  virtual void visitRoot(AstRoot* node);
  virtual void visitTopLevelStatement(AstStatement* node);
  virtual void visitStatements(AstStatement* node);
  virtual void visitStatement(AstStatement* node);
  virtual void visitStruct(AstStruct* node);
  virtual void visitStructField(AstStructField* node);
  virtual void visitType(TokenType type) { }
};

} // namespace hlsl
