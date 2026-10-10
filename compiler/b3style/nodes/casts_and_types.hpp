#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../code_gen/includes.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "../../logic/scope.hpp"
#include "../scope.hpp"
#include "../b3style_forward.hpp"
#include "struct_and_values.hpp"

#define DIALECT b3style_compiler
#define TYPEDEF_DELEGATE_TO_CHILD 1
#include "../../bstyle/nodes/casts_and_types.inc"
#undef TYPEDEF_DELEGATE_TO_CHILD
#undef DIALECT

