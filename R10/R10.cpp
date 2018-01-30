#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include "clang/Lex/LiteralSupport.h"

// Command line support
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include <string>

using namespace clang;

enum Category { BOOL, CHAR, SIGNED, UNSIGNED, ENUM, FLOAT, NONE };

// TODO what if typedef

// Essential type category
class EssentialT {

public:
  EssentialT(QualType T) : ClangType(T) {
    value = map(ClangType);
    llvm::outs() << EssentialT::getStr(value) << "\n\n";
    ;
  }

  // FIXME make sure you know all type function
  // Or know clang Types
  // clang/include/clang/AST/BuiltinTypes.def
  static enum Category map(QualType Ty) {

    const Type *T = Ty.getTypePtr();
    T->dump();
    if (!isa<BuiltinType>(Ty)) {
      if (T->isEnumeralType()) {
        return ENUM;
      }
      // llvm::outs() << "Not a built-in type\n";
      return NONE;
    }

    const BuiltinType *bt = dyn_cast<const BuiltinType>(T);

    if (T->isBooleanType()) {
      return BOOL;
    } else if ((bt->getKind() == BuiltinType::Char_U ||
                bt->getKind() == BuiltinType::Char_S)) {
      // unsigned char is also this type
      return CHAR;
    } else if (T->isSignedIntegerType()) {
      // signed char is also this type
      return SIGNED;
    } else if (T->isUnsignedIntegerType()) {
      // A complete enum is also this type
      return UNSIGNED;
    } else if (T->isFloatingType()) {
      return FLOAT;
    } else {
      return NONE;
    }
  }
  static std::string getStr(enum Category Cat) {
    switch (Cat) {
#define CASE(CAT)                                                              \
  case CAT:                                                                    \
    return #CAT;
      CASE(BOOL)
      CASE(CHAR)
      CASE(SIGNED)
      CASE(UNSIGNED)
      CASE(ENUM)
      CASE(FLOAT)
      CASE(NONE)
#undef CASE
    }
    return "No such Category";
  }

private:
  // QualType Type;
  QualType ClangType;
  enum Category value;
};

namespace R10 {

#include "pError.h"

class FindNamedClassVisitor
    : public RecursiveASTVisitor<FindNamedClassVisitor> {
public:
  explicit FindNamedClassVisitor(ASTContext *Context) : Context(Context) {}

  bool VisitVarDecl(VarDecl *vd) {
    QualType Type = vd->getType();
    EssentialT et(Type);
    // Type->dump();
    return true;
  }

private:
  ASTContext *Context;
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
      tooling::newFrontendActionFactory<R10::FindNamedClassAction>().get());

  return 0;
}
