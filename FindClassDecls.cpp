#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "clang/Lex/LiteralSupport.h"

// Command line support
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;

namespace test {

static void pLoc(FullSourceLoc& Loc,const char* msg) {
  llvm::outs() << msg << "  "
                     << Loc.getSpellingLineNumber() << ":"
                     << Loc.getSpellingColumnNumber() << "\n";
//  Loc.dump();
}




class FindNamedClassVisitor
  : public RecursiveASTVisitor<FindNamedClassVisitor> {
public:
  explicit FindNamedClassVisitor(ASTContext *Context)
    : Context(Context) {}


  //R12_5:  The sizeof operator shall not have an operand which is a function parameter declared as “array of type”
  bool VisitUnaryExprOrTypeTraitExpr(UnaryExprOrTypeTraitExpr *expr) {

    // Only Sizeof operator
    if (expr->getKind() != UETT_SizeOf) {
	  return false;
	}
    // Except sizeof(type)
    // Wipe out type is argument
    if (expr->isArgumentType() != false) {
      return false;
    }

    //expr->dumpColor();
    //
    Expr* arg = expr->getArgumentExpr();

		// Copy from clang/lib/Sema/SemaExpr.cpp
		if (DeclRefExpr *DeclRef = dyn_cast<DeclRefExpr>(arg->IgnoreParens())) {
			if (ParmVarDecl *PVD = dyn_cast<ParmVarDecl>(DeclRef->getFoundDecl())) {
				QualType OType = PVD->getOriginalType();
				QualType Type = PVD->getType();
				if (Type->isPointerType() && OType->isArrayType()) {

          FullSourceLoc FullLocation = Context->getFullLoc(DeclRef->getLocStart());
          if (FullLocation.isValid()) {
		        pLoc(FullLocation,"Using sizeof on function parameter!");
				  }
        }
			}
		}

    return true;
  }



	// R7_2 A "u" or "U" suffix shall be applied to all integer constants that are represented in an unsigned type
	bool VisitIntegerLiteral(IntegerLiteral *il) {
//		il->dumpColor();

    using std::string;
    const SourceManager &sm = Context->getSourceManager();
    const SourceLocation spellingLoc = sm.getSpellingLoc(il->getLocStart());
    const string lexem = srcLocToString(spellingLoc);
//    const NumericLiteralParser nlp(lexem, spellingLoc, CI->getPreprocessor());

//		llvm::outs() << lexem << "\n";

    /* if ((nlp.isUnsigned && lexem.find("u") != string::npos) ||
        (nlp.isFloat && lexem.find("f") != string::npos) ||
        ((nlp.isLong || nlp.isLongLong) && lexem.find("l") != string::npos)) {
      reportError(expr->getLocEnd());
    }
*/
    return true;
  }


private:
  ASTContext *Context;

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
  explicit FindNamedClassConsumer(ASTContext *Context)
    : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  FindNamedClassVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new FindNamedClassConsumer(&Compiler.getASTContext()));
  }
};

} // end of namespace test


// Handling input

static llvm::cl::OptionCategory MyToolCategory("my-tool options");
static llvm::cl::extrahelp CommonHelp(tooling::CommonOptionsParser::HelpMessage);

int main(int argc,const char **argv) {
  tooling::CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);

  tooling::ClangTool Tool(OptionsParser.getCompilations(),
                       OptionsParser.getSourcePathList());
  return Tool.run(tooling::newFrontendActionFactory<test::FindNamedClassAction >().get());

  /*  if (argc > 1) {
    clang::tooling::runToolOnCode(new test::FindNamedClassAction, argv[1]);
  }
*/

  return 0;
}

