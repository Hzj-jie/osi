#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../../../app_info/assert.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../logic/logic_writer.hpp"
#include "../scope.hpp"
#include "../b3style_forward.hpp"
#include "struct_and_values.hpp"
#include "functions.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class binary_operation_value_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "binary-operation-value"; }

                static std::string operation_function_name(std::string op)
                {
                    assert(!op.empty());
                    for (char& c : op)
                    {
                        if (c == '-') c = '_';
                    }
                    return namespace_t::fully_qualified_name("b2style", op);
                }

                static bool build(const std::shared_ptr<automata::typed_node>& n,
                                  const std::function<bool(const std::string&, logic_writer&)>& fc,
                                  logic_writer& o)
                {
                    assert(n != nullptr && n->child_count() == 3);
                    assert(fc != nullptr);
                    return value_list::build([&n](const std::function<void(const std::shared_ptr<automata::typed_node>&)>& a) {
                        assert(a != nullptr);
                        a(n->child(0));
                        a(n->child(2));
                    }, o) &&
                    fc(operation_function_name(n->child(1)->type_name), o);
                }

                static bool without_return(const std::shared_ptr<automata::typed_node>& n, logic_writer& o)
                {
                    return build(n, [](const std::string& name, logic_writer& oo) {
                        return function_call_node::ignore_parameters::without_return(name, oo);
                    }, o);
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    return build(n, [](const std::string& name, logic_writer& oo) {
                        return function_call_node::ignore_parameters::build(name, oo);
                    }, o);
                }
            };

            class unary_operation_value_node : public code_gen<logic_writer>
            {
            private:
                uint32_t operator_index_;
                std::string suffix_;

            public:
                static const char* node_name() { return "unary-operation-value"; }

                unary_operation_value_node(uint32_t operator_index, std::string suffix)
                    : operator_index_(operator_index), suffix_(std::move(suffix))
                {
                    assert(!suffix_.empty());
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && n->child_count() == 2);
                    std::string function_name = binary_operation_value_node::operation_function_name(
                        n->child(operator_index_)->type_name + suffix_);
                    auto operand = n->child(1 - operator_index_);
                    return value_list::build([&operand](const std::function<void(const std::shared_ptr<automata::typed_node>&)>& a) {
                        assert(a != nullptr);
                        a(operand);
                    }, o) &&
                    function_call_node::ignore_parameters::build(function_name, o);
                }
            };
        }
    }
}
