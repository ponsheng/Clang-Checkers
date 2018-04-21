#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "clang/Lex/LiteralSupport.h"

// Command line support
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include <map>
#include <string>

#include "R10.hpp"
#include "R_10_4.h"
#include "R_10_5.h"

using namespace clang;


namespace misrac {

#include "pError.h"

class FindNamedClassVisitor
    : public RecursiveASTVisitor<FindNamedClassVisitor> {
public:
  explicit FindNamedClassVisitor(ASTContext *Context) : Context(Context) {}

  /************************************************************************************
 - clang/include/clang/AST/OperationKinds.def

 To determine literal constant
 - warn_shift_negative
  - lib/Sema/SemaExpr.cp: 9067
************************************************************************************/

  // R10_1 Operands shall not be of an inappropriate essential type

  bool VisitArraySubscriptExpr(ArraySubscriptExpr *ase) {
    // ase->dumpColor();
    Expr *e = ase->getIdx()->IgnoreImplicit()->IgnoreParens();
    // e->dumpColor();

//    if (!op_table_lookup(e, "[]")) {
      pError(Context, e,
             "R10_1: Operands shall not be of an inappropriate essential type");
//    }
    return true;
  }

  bool VisitUnaryOperator(UnaryOperator *uo) {
    //Expr *e = uo->getSubExpr()->IgnoreImplicit()->IgnoreParens();
/*
    if (uo->getOpcode() == UO_Plus) {
      if (DeclRefExpr *dre = dyn_cast<DeclRefExpr>(e)) {
        QualType T = dre->getType();
        EssentialT et(T);
        if (et.value == BOOL || et.value == CHAR || et.value == ENUM) {
          pError(Context, e,
                 "R10_1: Operands of '+' shall not be of an inappropriate "
                 "essential "
                 "type");
        }
      }
    }*/

    if (uo->getOpcode() == UO_Minus) {
      //llvm::outs() << "MInus\n";
    }
    /*
      if (DeclRefExpr *dre = dyn_cast<DeclRefExpr>(e)) {
        QualType T = dre->getType();
        EssentialT et(T);
        if (et.value == BOOL || et.value == CHAR || et.value == ENUM ||
            et.value == UNSIGNED) {
          pError(Context, e,
                 "R10_1: Operands of '-' shall not be of an inappropriate
    essential " "type");
        }
      }
    }*/
    return true;
  }

      // R_10_2  
      // TODO The result of the operation has essentially
      // character type.?
      // What if chain operator 
      // _char = signed_char + _char + signed_char + unsigned_char
 

  bool VisitBinaryOperator(BinaryOperator* bo) {

      Expr *lhs = bo->getLHS()->IgnoreImplicit()->IgnoreParens();
      Expr *rhs = bo->getRHS()->IgnoreImplicit()->IgnoreParens();

      EssentialT lET(lhs);
      EssentialT rET(rhs);

      if ( bo->getOpcode() == BO_Add ) {
        //lhs->dumpColor();
        //lhs->dumpPretty(*Context);

        if ( lET.value == ET_CHAR ) {
          if ( rET.value != ET_SIGNED && rET.value != ET_UNSIGNED) {
          pError(Context, bo,
             "R10_2: Expressions of essentially character type shall not be used"
             "inappropriately in addition and subtraction operations");
          }
        } else if ( rET.value == ET_CHAR ) {
          if ( lET.value != ET_SIGNED && lET.value != ET_UNSIGNED) {
          pError(Context, bo,
             "R10_2: Expressions of essentially character type shall not be used"
             "inappropriately in addition and subtraction operations");
          }
        }

      }

      if (bo->getOpcode() == BO_Sub ) {
        if ( lET.value == ET_CHAR ) {
          if ( rET.value != ET_SIGNED && rET.value != ET_UNSIGNED && rET.value != ET_CHAR ) {
            pError(Context, bo,
             "R10_2: Expressions of essentially character type shall not be used"
             "inappropriately in addition and subtraction operations");
          }
        }
        if (rET.value == ET_CHAR) {
          if ( lET.value != ET_CHAR ) {
            pError(Context, bo,
             "R10_2: Expressions of essentially character type shall not be used"
             "inappropriately in addition and subtraction operations");
          }
        }
      }
      return true;
    }

private:
  ASTContext *Context;
};

class FindNamedClassConsumer : public clang::ASTConsumer {
public:
  explicit FindNamedClassConsumer(ASTContext *Context) : Visitor(Context) , Visitor_10_4(Context), Visitor_10_5(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    Visitor_10_4.TraverseDecl(Context.getTranslationUnitDecl());
    Visitor_10_5.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  FindNamedClassVisitor Visitor;
  R_10_4 Visitor_10_4;
  R_10_5 Visitor_10_5;
};

class FindNamedClassAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new FindNamedClassConsumer(&Compiler.getASTContext()));
  }
};

} // end of namespace R10

// Handling input

static llvm::cl::OptionCategory MyToolCategory("my-tool options");
static llvm::cl::extrahelp
    CommonHelp(tooling::CommonOptionsParser::HelpMessage);

int main(int argc, const char **argv) {
  tooling::CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);

  tooling::ClangTool Tool(OptionsParser.getCompilations(),
                          OptionsParser.getSourcePathList());
  return Tool.run(
      tooling::newFrontendActionFactory<misrac::FindNamedClassAction>().get());

  return 0;
}
