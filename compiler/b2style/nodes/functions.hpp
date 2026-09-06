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
            class function_node : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "function"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() >= 5);
                    std::string function_name = _namespace::bstyle_format::of(n->child(1));

                    scope child_scope(scope::current());
                    child_scope.current_function().define(function_name);

                    rewriter::typed_node_writer fo;
                    for (uint32_t i = 0; i < n->child_count(); ++i)
                    {
                        if (!code_gen_of(n->child(i)).build(fo))
                        {
                            return false;
                        }
                    }

                    auto fo_ptr = std::make_shared<rewriter::typed_node_writer>(std::move(fo));
                    o.append(scope::current()->call_hierarchy().filter(function_name, [fo_ptr]() {
                        return fo_ptr->str();
                    }));
                    return true;
                }
            };

            class function_call : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "function-call"; }

                static bool split_struct_function(const std::string& name, std::string& obj, std::string& func)
                {
                    size_t dot_pos = name.rfind('.');
                    if (dot_pos == std::string::npos || dot_pos == 0 || dot_pos + 1 >= name.length())
                    {
                        return false;
                    }
                    obj = name.substr(0, dot_pos);
                    func = name.substr(dot_pos + 1);
                    return true;
                }

                static bool build(const std::string& name,
                                  const std::shared_ptr<automata::typed_node>& n,
                                  rewriter::typed_node_writer& o)
                {
                    assert(!name.empty());
                    assert(n != nullptr);
                    assert(n->child_count() == 3 || n->child_count() == 4);

                    std::string obj, func;
                    if (!split_struct_function(name, obj, func))
                    {
                        if (scope::current()->variable_defined(name))
                        {
                            // Delegate call
                            for (uint32_t i = 0; i < n->child_count(); ++i)
                            {
                                if (!code_gen_of(n->child(i)).build(o))
                                {
                                    return false;
                                }
                            }
                            return true;
                        }

                        o.append(_namespace::bstyle_format::of(name));
                        scope::current()->call_hierarchy().to(name);
                        for (uint32_t i = 1; i < n->child_count(); ++i)
                        {
                            if (!code_gen_of(n->child(i)).build(o))
                            {
                                return false;
                            }
                        }
                        return true;
                    }

                    std::string function_name = _namespace::bstyle_format::fully_qualified_name(func);
                    scope::current()->call_hierarchy().to_bstyle_function(function_name);
                    o.append(function_name);
                    o.append("(");
                    o.append(_namespace::bstyle_format::of(obj));
                    if (n->child_count() == 4)
                    {
                        o.append(", ");
                        if (!code_gen_of(n->child(2)).build(o))
                        {
                            return false;
                        }
                    }
                    o.append(")");
                    return true;
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 3 || n->child_count() == 4);
                    return build(n->child(0)->input_without_ignored(), n, o);
                }
            };
        }
    }
}
