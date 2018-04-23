#ifndef _R10_H_
#define _R10_H_

#include "clang/AST/Type.h"
#include <string>

//#define R_10_VERBOSE

using namespace clang;

enum ET {
  ET_NONE = 0,
  ET_BOOL = 0x01,
  ET_CHAR = 0x02,
  ET_SIGNED = 0x04,
  ET_UNSIGNED = 0x08,
  ET_ENUM = 0x10,
  ET_FLOAT = 0x20
};

// TODO what if typedef

// Essential type category
class EssentialT {

public:
  EssentialT() : value(ET_NONE) {}

  EssentialT(Expr *e) : value(ET_NONE) { setET(e); }
  enum ET setET(Expr *e) {
    value = Expr_to_Essential(e);
    setType(e);

#ifdef R_10_VERBOSE
    e->dumpColor();
    llvm::outs() << EssentialT::getStr(value) << "\n\n";
#endif

    return value;
  }

  // This should be called after value set
  void setType(Expr *e) {
    //  Directly desugar? -> No We need info like type size
    type = e->getType().getTypePtr();

    if (value == ET_ENUM) {
      // For enum type def tyoe
      while (const TypedefType *tt = dyn_cast<TypedefType>(type)) {
        // if ( type->isSugared()) {
        type = tt->desugar().getTypePtr();
        //}
      }

      // For ElaboratedType enum, because we have to compare with
      // EnumConstantDecl
      if (const ElaboratedType *et = dyn_cast<ElaboratedType>(type)) {
        if (et->isSugared()) {
          type = et->desugar().getTypePtr();
        }
      }

      // For EnumConstantDecl
      if (DeclRefExpr *dre = dyn_cast<DeclRefExpr>(e)) {

        ValueDecl *nd = dre->getDecl();

        if (isa<EnumConstantDecl>(nd)) {

          if (EnumDecl *ed = dyn_cast<EnumDecl>(nd->getDeclContext())) {
            type = ed->getTypeForDecl();
          } else {
            llvm::outs() << "Failed to get type of [EnumConstantDecl]\n";
            nd->dumpColor();
          }
        }
      }
    }

#ifdef R_10_VERBOSE
    type->dump();
#endif
  }

  static enum ET Expr_to_Essential(Expr *e) {
    if (isa<DeclRefExpr>(e)) {

      DeclRefExpr *dre = dyn_cast<DeclRefExpr>(e);
      ValueDecl *nd = dre->getDecl();

      // Deal with EnumConstantDecl type record
      // TODO An enumeration constant from an anonymous enum
      // has essentially signed type.
      if (isa<EnumConstantDecl>(nd)) {

#ifdef R_10_VERBOSE
        llvm::outs() << "EnumConstantDecl\n";
        // EnumConstantDecl *ecd = dyn_cast<EnumConstantDecl>(nd);
        // ecd->dumpColor();
        if (EnumDecl *ed = dyn_cast<EnumDecl>(nd->getDeclContext())) {
          const Type *t = ed->getTypeForDecl();
          if (const TagType *tt = dyn_cast<TagType>(t)) {
            // llvm::outs() << tt->isBeingDefined() <<
            // tt->isCompleteDefinition()  << "\n";
          }
          if (TagDecl *td = dyn_cast<TagDecl>(ed)) {
            llvm::outs() << td->isBeingDefined() << td->isCompleteDefinition()
                         << "\n";
            if (TypedefNameDecl *tnd = td->getTypedefNameForAnonDecl()) {
              tnd->dump();
            }
          }
        }
#endif

        return ET_ENUM;
      }

      return QualType_to_Essential(dre->getType());
    }
    if (isa<IntegerLiteral>(e)) {
      return QualType_to_Essential(dyn_cast<IntegerLiteral>(e)->getType());
    }
    // Ignore Ctyle or not: No this include type wide info
    if (isa<CStyleCastExpr>(e)) {
      return QualType_to_Essential(dyn_cast<CStyleCastExpr>(e)->getType());
    }
    if (isa<CharacterLiteral>(e)) {
      return ET_CHAR;
    }
    if (isa<FloatingLiteral>(e)) {
      return ET_FLOAT;
    }
    if (ImplicitCastExpr *ice = dyn_cast<ImplicitCastExpr>(e)) {
      return Expr_to_Essential(ice->getSubExpr());
    }
    // Ignore check, direct getType

    return QualType_to_Essential(e->getType());

    return ET_NONE;
  }

  // We need functions to convert these CStyleCastExpr, IntegerLiteral,
  // DeclRefExpr

  // FIXME make sure you know all type function
  // Or know clang Types
  // clang/include/clang/AST/BuiltinTypes.def
  static enum ET QualType_to_Essential(QualType qt) {

    // QualType Ty = dre->getType();
    const Type *T = qt.getTypePtr();

    // FIXME Deal with sugar here  uint8_t
    if (!isa<BuiltinType>(T)) {

#ifdef R_10_VERBOSE
      llvm::outs() << "Not Built-in type\n";
#endif

      if (T->isEnumeralType()) {
        return ET_ENUM;
      }
      if (isa<TypedefType>(T)) {
        const TypedefType *tt = dyn_cast<TypedefType>(T);

        TypedefNameDecl *tnd = tt->getDecl();

        return QualType_to_Essential(tt->desugar());
      }

      llvm::outs() << "Unkonwn Non-built-in type\n";
      return ET_NONE;
    }

    // llvm::outs() << "Built-in type\n";
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
      // TODO 0.0f does not go here
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
  //  QualType qtype;
  const Type *type;

private:
  // QualType Type;
};

#endif
