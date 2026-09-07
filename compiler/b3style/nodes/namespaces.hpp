#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../../../app_info/assert.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../logic/logic_writer.hpp"
#include "../scope.hpp"
#include "../b3style_forward.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class namespace_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "namespace"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() >= 4);
                    assert(!_disable_namespace);
                    std::string ns_name = n->child(1)->input_without_ignored();
                    auto guard = scope::current()->current_namespace().define(
                        current_namespace_t::of(ns_name));
                    for (uint32_t i = 3; i < n->child_count() - 1; ++i)
                    {
                        if (!code_gen_of(n->child(i)).build(o))
                        {
                            return false;
                        }
                    }
                    return true;
                }
            };
        }
    }
}
