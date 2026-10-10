#include "../../../../compiler/b2style/b2style.hpp"
#include "../../../../compiler/bstyle/bstyle.hpp"

using namespace osi::compiler::b2style_compiler;

#define DIALECT_NAME "b2style"
#define COMPILER_WITH_BSTYLE_STAGE
#include "../../bstyle/bstyle_compiler/compiler_cli.inc"
