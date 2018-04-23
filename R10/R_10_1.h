#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include <string.h>

#include "R10.hpp"

using namespace clang;

#include "pError.h"

/*
 * Rule 10.1
 * Operands shall not be of an inappropriate essential type
 */

class R_10_1 : public RecursiveASTVisitor<R_10_1> {
public:
  explicit R_10_1(ASTContext *Context)
      : Context(Context),
        err_msg("Rule 10.1: "
                "Operands shall not be of an inappropriate essential type") {}

  // maybe either operand ?
  bool VisitArraySubscriptExpr(ArraySubscriptExpr *ase) {
    Expr *e = ase->getIdx()->IgnoreImplicit()->IgnoreParens();
    EssentialT et(e);

    if (et.value & (ET_BOOL | ET_CHAR | ET_FLOAT)) {
      pError(Context, e, err_msg);
    }
    return true;
  }

  bool VisitUnaryOperator(UnaryOperator *uo) {
    Expr *e = uo->getSubExpr()->IgnoreImplicit()->IgnoreParens();
    // e->dumpColor();
    EssentialT et(e);

    enum UnaryOperatorKind OPcode = uo->getOpcode();

    switch (OPcode) {
    case UO_Minus:
      if (et.value & (ET_BOOL | ET_CHAR | ET_ENUM | ET_UNSIGNED)) {
        pError(Context, uo, err_msg);
      }
      break;

    case UO_Plus:
      if (et.value & (ET_BOOL | ET_CHAR | ET_ENUM)) {
        pError(Context, uo, err_msg);
      }
      break;

    case UO_LNot:
      if (et.value & (ET_CHAR | ET_ENUM | ET_SIGNED | ET_UNSIGNED | ET_FLOAT)) {
        pError(Context, uo, err_msg);
      }
      break;

    case UO_Not:
      if (et.value & (ET_BOOL | ET_CHAR | ET_ENUM | ET_SIGNED | ET_FLOAT)) {
        pError(Context, uo, err_msg);
      }
      break;

    case UO_PostInc:
    case UO_PreInc:
    case UO_PostDec:
    case UO_PreDec:
      if (et.value & (ET_BOOL | ET_CHAR | ET_ENUM)) {
        pError(Context, uo, err_msg);
      }
      break;

    default:;
    }

    return true;
  }

  bool VisitBinaryOperator(BinaryOperator *bo) {
    Expr *lhs = bo->getLHS()->IgnoreImplicit()->IgnoreParens();
    Expr *rhs = bo->getRHS()->IgnoreImplicit()->IgnoreParens();

    EssentialT lET(lhs);
    EssentialT rET(rhs);

    enum BinaryOperatorKind OPcode = bo->getOpcode();

    switch (OPcode) {
    case BO_Add:
    case BO_AddAssign:
    case BO_Sub:
    case BO_SubAssign:

      if ((lET.value | rET.value) & (ET_BOOL | ET_ENUM)) {
        pError(Context, bo, err_msg);
      }
      break;

    case BO_Mul:
    case BO_MulAssign:
    case BO_Div:
    case BO_DivAssign:
      if ((lET.value | rET.value) & (ET_BOOL | ET_CHAR | ET_ENUM)) {
        pError(Context, bo, err_msg);
      }
      break;

    case BO_Rem:
    case BO_RemAssign:

      if ((lET.value | rET.value) & (ET_BOOL | ET_CHAR | ET_ENUM | ET_FLOAT)) {
        pError(Context, bo, err_msg);
      }
      break;

    case BO_LT:
    case BO_GT:
    case BO_LE:
    case BO_GE:

      if ((lET.value | rET.value) & (ET_BOOL)) {
        pError(Context, bo, err_msg);
      }
      break;

    case BO_LAnd:
    case BO_LOr:

      if ((lET.value | rET.value) &
          (ET_CHAR | ET_ENUM | ET_SIGNED | ET_UNSIGNED | ET_FLOAT)) {
        pError(Context, bo, err_msg);
      }
      break;

    case BO_Shl:
    case BO_Shr:

      // [Exception]
      // A non-negative integer constant expression of essentially signed type
      // may be used as the right hand operand to a shift operator.
      if (rET.value == ET_SIGNED) {
        if (IntegerLiteral *il = dyn_cast<IntegerLiteral>(rhs)) {
          llvm::APInt literal = il->getValue();
          uint64_t value = literal.getLimitedValue();
          if (value >= 0) {
            rET.value = ET_UNSIGNED;
          }
        }
      }

      if ((lET.value | rET.value) &
          (ET_BOOL | ET_CHAR | ET_ENUM | ET_SIGNED | ET_FLOAT)) {
        pError(Context, bo, err_msg);
      }
      break;

    case BO_And:
    case BO_Or:
    case BO_Xor:
      if ((lET.value | rET.value) &
          (ET_BOOL | ET_CHAR | ET_ENUM | ET_SIGNED | ET_FLOAT)) {
        pError(Context, bo, err_msg);
      }
      break;
    default:;
    }

    return true;
  }

  bool VisitConditionalOperator(ConditionalOperator *co) {
    Expr *cond = co->getCond()->IgnoreImplicit()->IgnoreParens();

    EssentialT ET(cond);

    if ((ET.value) & (ET_CHAR | ET_ENUM | ET_SIGNED | ET_UNSIGNED | ET_FLOAT)) {
      pError(Context, co, err_msg);
    }

    return true;
  }

private:
  ASTContext *Context;
  const std::string err_msg;
};
