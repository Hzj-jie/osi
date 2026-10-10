#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../scope.hpp"
#include "../b2style_forward.hpp"
#include "../../rewriter/typed_node_writer.hpp"
#include "../../code_gen/code_gen.hpp"
#include "functions.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../../app_info/assert.hpp"

#define DIALECT b2style_compiler
#define WRITER rewriter::typed_node_writer
#define TEMPLATE_TYPE_FORMAT(s) (_namespace::bstyle_format::of(s))
#define FUNCTION_CALL_BUILD(fn, node, oo) (function_call::build(fn, node, oo))
#include "templates.inc"
