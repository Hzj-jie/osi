#pragma once
#include <memory>
#include "../code_gen/code_gen.hpp"
#include "../logic/logic_writer.hpp"
#include "../../automata/typed_node.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            code_gens<logic_writer>& get_b3style_code_gens();

            inline code_gens<logic_writer>::code_gen_proxy code_gen_of(const std::shared_ptr<automata::typed_node>& n)
            {
                return get_b3style_code_gens().of(n);
            }

            bool build_code(const std::string& input, logic_writer& o);
        }
    }
}
