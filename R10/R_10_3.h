#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include <string.h>

#include "R10.hpp"

using namespace clang;

#include "pError.h"

/*
 * Rule 10.3
 * The value of an exp ression shall not be assigne d to an object with a
 * narrower essential type or of a diff erent essential type category
 */

class R_10_3 : public RecursiveASTVisitor<R_10_3> {
public:
  explicit R_10_3(ASTContext *Context)
      : Context(Context),
        err_msg("Rule 10.3 "

                " The value of an exp ression shall not be assigne d to an "
                "object with a narrower "
                "essential type or of a diff erent essential type category ") {}

  bool VisitBinaryOperator(BinaryOperator *bo) {

    // Difficult to deal with += -= ..
    if (bo->getOpcode() == BO_Assign) {
      Expr *lhs = bo->getLHS()->IgnoreImplicit()->IgnoreParens();
      Expr *rhs = bo->getRHS()->IgnoreImplicit()->IgnoreParens();

      EssentialT lET(lhs);
      EssentialT rET(rhs);

      // pError(Context, bo, err_msg);
    }

    return true;
  }

private:
  ASTContext *Context;
  const std::string err_msg;
};
