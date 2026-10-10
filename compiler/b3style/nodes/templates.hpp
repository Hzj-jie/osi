#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../logic/logic_writer.hpp"
#include "../scope.hpp"
#include "../b3style_forward.hpp"
#include "functions.hpp"

#define DIALECT b3style_compiler
#define WRITER logic_writer
#define TEMPLATE_TYPE_FORMAT(s) (s)
#define FUNCTION_CALL_BUILD(fn, node, oo) (function_call_node::build(fn, node, oo))
#define FUNCTION_CALL_WITHOUT_RETURN(fn, node, oo) (function_call_node::with_parameters::without_return(fn, node, oo))
#include "../../b2style/nodes/templates.inc"
