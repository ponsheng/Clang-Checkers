
# Not sure what's this line mean for
set(LLVM_LINK_COMPONENTS support)

include_directories(include)

add_clang_executable(find-class-decls FindClassDecls.cpp)

target_link_libraries(find-class-decls clangTooling)
add_subdirectory(R10)
