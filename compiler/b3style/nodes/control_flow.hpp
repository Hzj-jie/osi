#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "../scope.hpp"
#include "../b3style_forward.hpp"

#define DIALECT b3style_compiler
#include "../../bstyle/nodes/control_flow.inc"
#undef DIALECT
