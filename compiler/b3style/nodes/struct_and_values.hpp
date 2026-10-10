#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "../../logic/scope.hpp"
#include "../../logic/variable.hpp"
#include "../scope.hpp"
#include "../b3style_forward.hpp"
#include "raw_value.hpp"
#include "logic_name.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
#define DIALECT b3style
#define VALUE_DECLARATION_CUSTOM 1
#include "../../bstyle/nodes/struct_and_values.inc"
#undef VALUE_DECLARATION_CUSTOM
#undef DIALECT
        }
    }
}
