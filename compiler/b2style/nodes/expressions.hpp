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
            class binary_operation_value : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "binary-operation-value"; }

                static bool build_binary(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o)
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 3);
                    std::string function_name = _namespace::bstyle_format::operator_function_name(n->child(1)->type_name);
                    scope::current()->call_hierarchy().to_bstyle_function(function_name);
                    o.append(function_name);
                    o.append("(");
                    if (!code_gen_of(n->child(0)).build(o))
                    {
                        return false;
                    }
                    o.append(",");
                    if (!code_gen_of(n->child(2)).build(o))
                    {
                        return false;
                    }
                    o.append(")");
                    return true;
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    return build_binary(n, o);
                }
            };

            class unary_operation_value : public code_gen<rewriter::typed_node_writer>
            {
            private:
                uint32_t operator_index_;
                std::string suffix_;

            public:
                unary_operation_value(uint32_t op_index, std::string suffix)
                    : operator_index_(op_index), suffix_(std::move(suffix))
                {
                    assert(!suffix_.empty());
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 2);
                    std::string function_name =
                        _namespace::bstyle_format::operator_function_name(n->child(operator_index_)->type_name) + suffix_;
                    scope::current()->call_hierarchy().to_bstyle_function(function_name);
                    o.append(function_name);
                    o.append("(");
                    if (!code_gen_of(n->child(1 - operator_index_)).build(o))
                    {
                        return false;
                    }
                    o.append(")");
                    return true;
                }
            };
        }
    }
}
