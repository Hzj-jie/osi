#include "../../../../compiler/bstyle/bstyle.hpp"

using namespace osi::compiler::bstyle_compiler;

#define DIALECT bstyle
#define DIALECT_NAME "bstyle"
#define RUNNER_IS_EXTENSION(ext) ((ext) == ".bstyle" || (ext) == ".txt")
#include "runner.inc"
