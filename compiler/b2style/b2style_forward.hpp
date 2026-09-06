#pragma once
#include <memory>
#include "../code_gen/code_gen.hpp"
#include "../rewriter/typed_node_writer.hpp"
#include "../../automata/typed_node.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {
            code_gens<rewriter::typed_node_writer>& get_b2style_code_gens();

            inline code_gens<rewriter::typed_node_writer>::code_gen_proxy code_gen_of(const std::shared_ptr<automata::typed_node>& n)
            {
                return get_b2style_code_gens().of(n);
            }

            bool build_code(const std::string& input, rewriter::typed_node_writer& o);
        }
    }
}
