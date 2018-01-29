
template <class T> void pError(ASTContext *Context, T ASTnode, const char *msg) {
  const SourceManager &sm = Context->getSourceManager();
  const SourceLocation LocStart = ASTnode->getLocStart();
  const SourceLocation SpellingLoc = sm.getSpellingLoc(ASTnode->getLocStart());
  // const SourceLocation FileLoc = sm.getFileLoc(ASTnode->getLocStart());

  // FullSourceLoc is just combination SM & SourceLocation
  // FullSourceLoc FullLocation = Context->getFullLoc(ASTnode->getLocStart());

  if (LocStart.isValid()) {
    //    LocStart.dump(sm);
    SpellingLoc.dump(sm);
    //    FileLoc.dump(sm);
    llvm::outs() << "\n - " << msg << "\n";
    /*llvm::outs() << sm.getFilename(spellingLoc) << ":"
                 << sm.getSpellingLineNumber(spellingLoc) << ":"
                 << sm.getSpellingColumnNumber(spellingLoc) << "  " << msg
                 << "  "
                 << "\n";*/
  } else {
    llvm::outs() << "FullSourceLoc is not valid\n";
    return;
  }

  if (LocStart.isMacroID() == true) {
    llvm::outs() << "Defined in Macro!!\n";
  } else {
    // llvm::outs() << "Not macro\n";
  }


  llvm::outs() << "\n";
}
