#include "clang/AST/Type.h"

#include <string>

using namespace clang;

enum ET { ET_NONE, ET_BOOL, ET_CHAR, ET_SIGNED, ET_UNSIGNED, ET_ENUM, ET_FLOAT};

// TODO what if typedef

// Essential type category
class EssentialT {

public:
  EssentialT(Expr* e) : value(ET_NONE) {
    value = Expr_to_Essential(e);
    //    llvm::outs() << EssentialT::getStr(value) << "\n\n";
  }

  EssentialT() : value(ET_NONE) {}

  enum ET setET(Expr* e)
  {
    return value = Expr_to_Essential(e);
    //ClangType = T;
  }

  static enum ET Expr_to_Essential(Expr* e) {
    if ( isa<DeclRefExpr>(e) ) {
      return QualType_to_Essential( dyn_cast<DeclRefExpr>(e)->getType() );
    }
    if ( isa<IntegerLiteral>(e)) {
      return QualType_to_Essential( dyn_cast<IntegerLiteral>(e)->getType() ); 
    }
    // FIXME ignore Ctyle or not
    if ( isa<CStyleCastExpr>(e)) {
      return QualType_to_Essential( dyn_cast<CStyleCastExpr>(e)->getType() ); 
    }
    if ( isa<CharacterLiteral>(e)) {
      //llvm::outs() << "Char\n";
      return  ET_CHAR;
      //QualType_to_Essential( dyn_cast<CharacterLiteral>(e)->getType() ); 
    }
    return ET_NONE;
  }

// We need functions to convert these CStyleCastExpr, IntegerLiteral, DeclRefExpr 

  // FIXME make sure you know all type function
  // Or know clang Types
  // clang/include/clang/AST/BuiltinTypes.def
  static enum ET QualType_to_Essential(QualType qt) {

   // QualType Ty = dre->getType();
    const Type *T = qt.getTypePtr();

    // FIXME Deal with sugar here  uint8_t
    if (!isa<BuiltinType>(T)) {
//      llvm::outs() << "Not Built-in type\n";
      if (T->isEnumeralType()) {
        return ET_ENUM;
      }
        if ( isa<TypedefType>(T)) {
          const TypedefType* tt = dyn_cast<TypedefType>(T);
           //llvm::outs() << "TypedefType type" << (tt->isSugared() ? " Sugar" : "")  << "\n";
            return QualType_to_Essential(tt->desugar());
        }

       T->dump();
       llvm::outs() << "Unkonwn Non-built-in type\n";
       return ET_NONE;
    }


    //llvm::outs() << "Built-in type\n";
    const BuiltinType *bt = dyn_cast<const BuiltinType>(T);

    if (T->isBooleanType()) {
      return ET_BOOL;
    } else if ((bt->getKind() == BuiltinType::Char_U ||
                bt->getKind() == BuiltinType::Char_S)) {
      // unsigned char is also this type
      return ET_CHAR;
    } else if (T->isSignedIntegerType()) {
      // signed char is also this type
      return ET_SIGNED;
    } else if (T->isUnsignedIntegerType()) {
      // A complete enum is also this type
      return ET_UNSIGNED;
    } else if (T->isFloatingType()) {
      return ET_FLOAT;
    } else {
      return ET_NONE;
    }
  }
  static std::string getStr(enum ET Cat) {
    switch (Cat) {
#define CASE(CAT)                                                              \
  case CAT:                                                                    \
    return #CAT;
      CASE(ET_BOOL)
      CASE(ET_CHAR)
      CASE(ET_SIGNED)
      CASE(ET_UNSIGNED)
      CASE(ET_ENUM)
      CASE(ET_FLOAT)
      CASE(ET_NONE)
#undef CASE
    }
    return "No such Category";
  }
  enum ET value;

private:
  // QualType Type;
  QualType ClangType;
};
