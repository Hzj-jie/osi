#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../../../app_info/assert.hpp"
#include "../../code_gen/statements.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "raw_value.hpp"
#include "../scope.hpp"

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            class code_types : public statement<logic_writer>
            {
            public:
                void export_to(logic_writer& o) override
                {
                    assert(builders::of_type(integer_node::type_name, 4).to(o));
                    assert(builders::of_type(bool_node::type_name, 1).to(o));
                    assert(builders::of_type(biguint_node::type_name, UINT32_MAX - 1).to(o));
                    assert(builders::of_type(ufloat_node::type_name, UINT32_MAX - 2).to(o));
                    assert(builders::of_type(string_node::type_name, UINT32_MAX - 3).to(o));
                }

                static void register_statement(statements<logic_writer>& p)
                {
                    p.register_statement(std::make_shared<code_types>());
                }
            };

            class main_caller : public statement<logic_writer>
            {
            public:
                void export_to(logic_writer& o) override
                {
                    assert(builders::of_caller("main", std::vector<std::string>()).to(o));
                }

                static void register_statement(statements<logic_writer>& p)
                {
                    p.register_statement(std::make_shared<main_caller>());
                }
            };
        }
    }
}
