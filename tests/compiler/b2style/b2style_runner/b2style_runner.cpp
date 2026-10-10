#include "../../../../compiler/b2style/b2style.hpp"

using namespace osi::compiler::b2style_compiler;

#define DIALECT b2style
#define DIALECT_NAME "b2style"
#define RUNNER_IS_EXTENSION(ext) ((ext) == ".b2style" || (ext) == ".cc" || (ext) == ".txt")
#include "../../bstyle/bstyle_runner/runner.inc"
