#pragma once
#include <memory>
#include <string>
#include <sstream>
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
            class heap_struct_name : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "heap-struct-name"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 3);
                    auto heap_name_node = n->child(0);
                    assert(heap_name_node->child_count() == 4);

                    std::string struct_name = heap_name_node->child(0)->input_without_ignored();
                    std::string field_name = n->child(2)->input_without_ignored();
                    std::string index_str = heap_name_node->child(2)->input_without_ignored();

                    std::string res = struct_name + "." + field_name + "[" + index_str + "]";
                    o.append(res);
                    return true;
                }
            };

            class class_initializer : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "class-initializer"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 4 || n->child_count() == 5);

                    std::string type_str = n->child(0)->input_without_ignored();
                    std::string var_str = n->child(1)->input_without_ignored();

                    if (!scope::current()->variables().define(type_str, var_str))
                    {
                        return false;
                    }

                    // type var;
                    if (!code_gen_of(n->child(0)).build(o) ||
                        !code_gen_of(n->child(1)).build(o) ||
                        !o.append(";"))
                    {
                        return false;
                    }

                    // construct(var, [args]);
                    std::string construct_fn = _namespace::bstyle_format::fully_qualified_name(class_def::construct);
                    if (!o.append(construct_fn) ||
                        !o.append("(") ||
                        !o.append(_namespace::bstyle_format::of(n->child(1))))
                    {
                        return false;
                    }
                    if (n->child_count() == 5)
                    {
                        if (!o.append(",") || !code_gen_of(n->child(3)).build(o))
                        {
                            return false;
                        }
                    }
                    if (!o.append(");"))
                    {
                        return false;
                    }

                    // Register destructor on when_end_scope
                    std::string var_formatted = _namespace::bstyle_format::of(n->child(1));
                    std::string destruct_fn = _namespace::bstyle_format::fully_qualified_name(class_def::destruct);
                    scope::current()->when_end_scope([&o, destruct_fn, var_formatted]() {
                        o.append(destruct_fn + "(" + var_formatted + ");");
                    });

                    scope::current()->call_hierarchy().to(namespace_t::fully_qualified_name(class_def::construct));
                    scope::current()->call_hierarchy().to(namespace_t::fully_qualified_name(class_def::destruct));
                    return true;
                }
            };
        }
    }
}

#define DIALECT b2style_compiler
#define WRITER rewriter::typed_node_writer
#define CLASS_VAR_TYPE_OF(n) ((n)->input_without_ignored())
#include "classes.inc"
