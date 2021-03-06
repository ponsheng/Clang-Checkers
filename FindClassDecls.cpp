#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "clang/Lex/LiteralSupport.h"

// Command line support
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include <algorithm>
#include <vector>

using namespace clang;

namespace test {

#include "pError.h"

class FindNamedClassVisitor
    : public RecursiveASTVisitor<FindNamedClassVisitor> {
public:
  explicit FindNamedClassVisitor(ASTContext *Context) : Context(Context) {}

  /**********************************************************************************/
  // R12_5:  The sizeof operator shall not have an operand which is a function
  // parameter declared as “array of type”
  bool VisitUnaryExprOrTypeTraitExpr(UnaryExprOrTypeTraitExpr *expr) {

    // Only Sizeof operator
    if (expr->getKind() != UETT_SizeOf) {
      return true;
    }
    // Except sizeof(type)
    // Wipe out type is argument
    if (expr->isArgumentType() != false) {
      return true;
    }

    // expr->dumpColor();
    //
    Expr *arg = expr->getArgumentExpr();

    // Copy from clang/lib/Sema/SemaExpr.cpp
    if (DeclRefExpr *DeclRef = dyn_cast<DeclRefExpr>(arg->IgnoreParens())) {
      if (ParmVarDecl *PVD = dyn_cast<ParmVarDecl>(DeclRef->getFoundDecl())) {
        QualType OType = PVD->getOriginalType();
        QualType Type = PVD->getType();
        if (Type->isPointerType() && OType->isArrayType()) {

          pError(Context, DeclRef,
                 "R12_5: Using sizeof on function parameter!");
        }
      }
    }

    return true;
  }

  /**********************************************************************************/
  // Code that can fetch token source code
  bool VisitIntegerLiteral(IntegerLiteral *il) {
    //		il->dumpColor();

    using std::string;
    const SourceManager &sm = Context->getSourceManager();
    const SourceLocation spellingLoc = sm.getSpellingLoc(il->getLocStart());
    const string lexem = srcLocToString(spellingLoc);
    //    const NumericLiteralParser nlp(lexem, spellingLoc,
    //    CI->getPreprocessor());

    // llvm::outs() << lexem << "\n";

    /* if ((nlp.isUnsigned && lexem.find("u") != string::npos) ||
        (nlp.isFloat && lexem.find("f") != string::npos) ||
        ((nlp.isLong || nlp.isLongLong) && lexem.find("l") != string::npos)) {
      reportError(expr->getLocEnd());
    }
*/
    return true;
  }

  /**********************************************************************************/
  // R12_3:  The comma operator should not be used
  bool VisitBinaryOperator(BinaryOperator *bo) {

    if (bo->getOpcode() != BO_Comma) {
      return true;
    }
    // bo->dumpColor();
    pError(Context, bo, "R12_3: Using comma operator!");
    return true;
  }

  /*********************************************************************************/
  // Rule 16_2 A switch label shall only be used when the most closely-enclosing
  //   compound statement is the body of a switch statement

  // TODO
  // Rule 16_3 An unconditional break statement shall terminate every
  // switch-clause

  //
  bool VisitSwitchStmt(SwitchStmt *ss) {
    CompoundStmt *body = dyn_cast<CompoundStmt>(ss->getBody());

    // There is always a compoundStmt even a empty switch
    if (body == nullptr) {
      return true;
    }

    std::vector<SwitchCase *> cases;
    // Record most closely-enclosing cases i.e body of the switch
    //
    for (CompoundStmt::body_iterator bi = body->body_begin();
         bi != body->body_end(); bi++) {
      //(*bi)->dumpColor();
      if (isa<SwitchCase>(*bi)) {
        SwitchCase *sc = dyn_cast<SwitchCase>(*bi);
        cases.push_back(sc);

        // If continuous case: case 1: case 2: case 3:
        while (isa<SwitchCase>(sc->getSubStmt())) {
          sc = dyn_cast<SwitchCase>(sc->getSubStmt());
          cases.push_back(sc);
        }
      }
    }

    for (const SwitchCase *sc = ss->getSwitchCaseList(); sc != nullptr;
         sc = sc->getNextSwitchCase()) {
      //
      // sc->dumpColor();
      if (std::find(cases.begin(), cases.end(), sc) == cases.end()) {

        pError(Context, sc,
               "R16_2: Switch case is not used in most closely-enclosing of "
               "switch body");
      }
    }

    /*******************************************************************************/
    // Rule 16_4 Every switch statement shall have a default label
    bool hasDefault = false;
    for (const SwitchCase *sc = ss->getSwitchCaseList(); sc != nullptr;
         sc = sc->getNextSwitchCase()) {
      if (isa<DefaultStmt>(sc)) {
        hasDefault = true;
      }
    }
    if (!hasDefault) {
      pError(Context, ss,
             "R16_4 Every switch statement shall have a default label");
    }

    /*******************************************************************************/
    // Rule 16_5 A default label shall appear as either the first or the last
    // switch label of a switch statement
    //
    // We check if any SwitchCase is default except first and end
    //
    // We just count the label and store in member
    // We let treaverse*() do the traverse AST work for us
    // See VisitSwitchCase() do the other jobs
    {
      // Reset class member in every switch
      labelTotal = 0;
      labelCount = 0;

      for (const SwitchCase *sc = ss->getSwitchCaseList(); sc != nullptr;
           sc = sc->getNextSwitchCase()) {
        labelTotal++;
      }

    } // end of R16_5

    /*******************************************************************************/
    // R16_6 Every switch statement shall have at least two switch-clauses
    {
      int labelCount = 0;
      for (const SwitchCase *sc = ss->getSwitchCaseList(); sc != nullptr;
           sc = sc->getNextSwitchCase()) {
        labelCount++;
      }
      if (labelCount < 2) {
        pError(Context, ss,
               "R16_6 Every switch statement shall have at least two "
               "switch-clauses");
      }
    }

    /*******************************************************************************/
    // R16_7 A switch-expression shall not have essentially Boolean type
    // Copy from clang/lib/Sema/SemaStmt.cpp
    {
      Expr *cond = ss->getCond();
      // cond->dumpColor();
      QualType CondType = cond->getType();

      // No need to check isTypeDependent() -> C has no template

      if (cond->isKnownToHaveBooleanValue()) {
        pError(Context, ss,
               "R16_7 A switch-expression shall not have essentially Boolean "
               "type");
      }
    }

    return true;
  } // end visitSwitch

  bool VisitSwitchCase(SwitchCase *sc) {
    labelCount++;
    // Defautlt statment can only appear in first or last
    if (labelCount != 1 && labelCount != labelTotal) {
      if (isa<DefaultStmt>(sc)) {
        pError(Context, sc,
               "R16_5: A default label shall appear as either the first "
               "or the last switch label");
      }
    }
    return true;
  }

private:
  ASTContext *Context;
  int labelCount, labelTotal;

  std::string srcLocToString(const SourceLocation start) {
    const clang::SourceManager &sm = Context->getSourceManager();
    const clang::LangOptions lopt = Context->getLangOpts();
    const SourceLocation spellingLoc = sm.getSpellingLoc(start);
    unsigned tokenLength =
        clang::Lexer::MeasureTokenLength(spellingLoc, sm, lopt);
    return std::string(sm.getCharacterData(spellingLoc),
                       sm.getCharacterData(spellingLoc) + tokenLength);
  }
};

class FindNamedClassConsumer : public clang::ASTConsumer {
public:
  explicit FindNamedClassConsumer(ASTContext *Context) : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  FindNamedClassVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new FindNamedClassConsumer(&Compiler.getASTContext()));
  }
};

} // end of namespace test

// Handling input

static llvm::cl::OptionCategory MyToolCategory("my-tool options");
static llvm::cl::extrahelp
    CommonHelp(tooling::CommonOptionsParser::HelpMessage);

int main(int argc, const char **argv) {
  tooling::CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);

  tooling::ClangTool Tool(OptionsParser.getCompilations(),
                          OptionsParser.getSourcePathList());
  return Tool.run(
      tooling::newFrontendActionFactory<test::FindNamedClassAction>().get());

  /*  if (argc > 1) {
    clang::tooling::runToolOnCode(new test::FindNamedClassAction, argv[1]);
  }
*/

  return 0;
}
