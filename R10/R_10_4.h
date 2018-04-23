#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"

#include "R10.hpp"

using namespace clang;

#include "pError.h"

/*
 * Both operands of an operator in which the usual arithmetic conversions
 * are performed shall have the same essential type category
 */

class R_10_4 : public RecursiveASTVisitor<R_10_4> {
public:
  explicit R_10_4(ASTContext *Context) : Context(Context) {}

  bool VisitBinaryOperator(BinaryOperator *bo) {

    // Excluded op
    // excluding the shift, logical &&, logical || and comma operators.
    // We also remove C++ OP, Bitwise , assign
    // TODO In addition, the second and third operands of the ternary operator
    //
    enum BinaryOperatorKind excluded_op[] = {BO_Comma,   BO_LOr,   BO_LAnd,
                                             BO_Shl,     BO_Shr,   BO_PtrMemD,
                                             BO_PtrMemI, BO_Assign};
    for (auto kind : excluded_op) {
      if (bo->getOpcode() == kind)
        return true;
    }

    Expr *lhs = bo->getLHS()->IgnoreImplicit()->IgnoreParens();
    Expr *rhs = bo->getRHS()->IgnoreImplicit()->IgnoreParens();

    EssentialT lET(lhs);
    EssentialT rET(rhs);

    /*
     * The following are permitted to allow a common form of charaicter
     * manipulation to be used:
     *
     * 1. The binary + and += operators may have one operand with essentially
     *    character type and the other operand with an essentially signed or
     *    essentially unsigned type;
     * 2. The binary - and -= operators may have a left-hand operand with
     *    essentially character type and a right-hand operand with an
     * essentially signed or essentially unsigned type.
     */

    if (bo->getOpcode() == BO_Add || bo->getOpcode() == BO_AddAssign) {
      if (rET.value & ET_CHAR && lET.value & (ET_UNSIGNED | ET_SIGNED)) {
        return true;
      }
      if (lET.value & ET_CHAR && rET.value & (ET_UNSIGNED | ET_SIGNED)) {
        return true;
      }
    }

    if (bo->getOpcode() == BO_Sub || bo->getOpcode() == BO_SubAssign) {
      if (rET.value & ET_CHAR && lET.value & (ET_UNSIGNED | ET_SIGNED)) {
        return true;
      }
      if (lET.value & ET_CHAR && rET.value & (ET_UNSIGNED | ET_SIGNED)) {
        return true;
      }
    }

    if (rET.value != lET.value) {
      // lhs->dumpColor();
      // lhs->dumpPretty(*Context);
      pError(
          Context, bo,
          "R10_4: Both operands of a n operator in which the usual arithmetic "
          "conversions are performed shall have the same essential type "
          "category");
    } else {
      if (rET.value == ET_ENUM) {
        if (rET.type->isElaboratedTypeSpecifier()) {
          // rET.type->dump();
        }
        if (lET.type->isElaboratedTypeSpecifier()) {
          // lET.type->dump();
        }

        if (rET.type != lET.type) {
          pError(Context, bo,
                 "R10_4: Both operands of a n operator in which the usual "
                 "arithmetic "
                 "conversions are performed shall have the same essential type "
                 "category");
        }
      }
    }

    return true;
  }

private:
  ASTContext *Context;
};
