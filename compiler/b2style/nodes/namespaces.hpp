#pragma once
#include <memory>
#include <string>
#include "../scope.hpp"
#include "../b2style_forward.hpp"
#include "../../rewriter/typed_node_writer.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {
            class namespace_node : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "namespace"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() >= 4);
                    std::string ns_name = n->child(1)->input_without_ignored();
                    auto guard = scope::current()->current_namespace().define(current_namespace_t::of(ns_name));
                    for (uint32_t i = 3; i + 1 < n->child_count(); ++i)
                    {
                        if (!code_gen_of(n->child(i)).build(o))
                        {
                            return false;
                        }
                    }
                    return true;
                }
            };

            class name_node : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "name"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    if (n->type_name == "name" && n->descendant_of(std::vector<std::string>{"value-declaration", "struct-body"}))
                    {
                        o.append(_namespace::bstyle_format::fully_qualified_name(n->input_without_ignored()));
                        return true;
                    }
                    if (n->type_name == "raw-type-name" &&
                        n->descendant_of(std::vector<std::string>{"paramtype"}) &&
                        (n->descendant_of(std::vector<std::string>{"template-type-name"}) ||
                         n->descendant_of(std::vector<std::string>{"function-name-with-template"})))
                    {
                        o.append(current_namespace_t::of(n->input_without_ignored()));
                        return true;
                    }
                    o.append(_namespace::bstyle_format::of(n));
                    return true;
                }
            };

            class paramtype_with_comma : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "paramtype-with-comma"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    if (n->descendant_of(std::vector<std::string>{"template-type-name"}) ||
                        n->descendant_of(std::vector<std::string>{"function-name-with-template"}))
                    {
                        return code_gen_of(n->child(0)).build(o);
                    }
                    for (uint32_t i = 0; i < n->child_count(); ++i)
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
