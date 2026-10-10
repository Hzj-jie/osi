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
#include "../../logic/variable.hpp"
#include "../../logic/scope.hpp"
#include "../scope.hpp"
#include "../../scope/struct_def.hpp"
#include "raw_value.hpp"
#include "logic_name.hpp"
#include "../bstyle_forward.hpp"

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
#define DIALECT bstyle
#define STRUCT_NAME_OF(node) scope::normalized_type::parameter_type_of(node).full_type()
#include "struct_and_values.inc"
#undef DIALECT
        }
    }
}
