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
#include "../../logic/scope.hpp"
#include "../../function_signature.hpp"
#include "../scope.hpp"
#include "../b3style_forward.hpp"
#include "struct_and_values.hpp"
#include "logic_name.hpp"
#include "raw_value.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
#define DIALECT b3style
#define FUNCTION_CALL_SUPPORT_STRUCT_FUNC
#include "../../bstyle/nodes/functions.inc"
#undef FUNCTION_CALL_SUPPORT_STRUCT_FUNC
#undef DIALECT
        }
    }
}
