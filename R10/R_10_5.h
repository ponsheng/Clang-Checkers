#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include <string.h>

#include "R10.hpp"

using namespace clang;

#include "pError.h"

/*
 * The value of an expression should not be cast to an inappropriate
 * essential ty pe
 */

class R_10_5 : public RecursiveASTVisitor<R_10_5> {
public:
  explicit R_10_5(ASTContext *Context)
      : Context(Context), err_msg("The value of an expression should not be "
                                  "cast to an inappropriate essential type") {}

  bool VisitCStyleCastExpr(CStyleCastExpr *csce) {
    // csce->dumpColor();
    // csce->getType().dump();

    // llvm::outs() << EssentialT::getStr(EssentialT::Expr_to_Essential(csce))
    // << "\n";
    EssentialT to(csce);
    Expr *sube = csce->getSubExpr();
    EssentialT from(sube);

    // TODO C99 'false' is essentially Boolean
    // Exception
    // An integer constant expression with the value 0 or 1 of either signedness
    // may be cast to a type which is defined as essentially Boolean.
    // This allows the implementation of non-C99 Boolean models.

    if (IntegerLiteral *il = dyn_cast<IntegerLiteral>(sube)) {
      llvm::APInt literal = il->getValue();
      // llvm::outs() << literal.getLimitedValue() << "\n" ;
      uint64_t value = literal.getLimitedValue();
      if (value == 1 || value == 0) {
        from.value = ET_BOOL;
        // Note type haven't changed
        // from.type = ;
      }
    }

    switch (from.value) {
    case ET_BOOL:
      if (to.value & (ET_CHAR | ET_SIGNED | ET_UNSIGNED | ET_ENUM | ET_FLOAT)) {
        pError(Context, csce, err_msg);
      }
      break;
    case ET_CHAR:
      if (to.value & (ET_BOOL | ET_ENUM | ET_FLOAT)) {
        pError(Context, csce, err_msg);
      }
      break;

    case ET_ENUM:
      if (to.value == ET_BOOL) {
        pError(Context, csce, err_msg);
      } else if (to.value == ET_ENUM && to.type != from.type) {
        pError(Context, csce, err_msg);
      }

      break;

    case ET_SIGNED:
      if (to.value & (ET_BOOL | ET_ENUM)) {
        pError(Context, csce, err_msg);
      }
      break;

    case ET_UNSIGNED:
      if (to.value & (ET_BOOL | ET_ENUM)) {
        pError(Context, csce, err_msg);
      }
      break;

    case ET_FLOAT:
      if (to.value & (ET_BOOL | ET_CHAR | ET_ENUM)) {
        pError(Context, csce, err_msg);
      }
      break;

    default:
      llvm::outs() << "Unkown essntial type\n";
      from.type->dump();
      csce->getSubExpr()->dumpColor();
    }

    return true;
  }

private:
  ASTContext *Context;
  const std::string err_msg;
};
