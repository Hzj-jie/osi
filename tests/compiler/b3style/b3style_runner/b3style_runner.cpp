#include "../../../../compiler/b3style/b3style.hpp"

using namespace osi::compiler::b3style_compiler;

#define DIALECT b3style
#define DIALECT_NAME "b3style"
#define RUNNER_IS_EXTENSION(ext) ((ext) == ".b3style" || (ext) == ".cc" || (ext) == ".txt")
#include "../../bstyle/bstyle_runner/runner.inc"
