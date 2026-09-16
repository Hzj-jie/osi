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
            class struct_node;
            class raw_variable_name;
            class heap_name;
            class heap_struct_name_node;
            class value_declaration;
            class heap_declaration;
            class value_clause;
            class value_list;
            class name_node;

            class struct_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "struct"; }

                static std::vector<parameter> parse_struct_body(const std::shared_ptr<automata::typed_node>& n)
                {
                    assert(n != nullptr);
                    std::vector<parameter> res;
                    for (const auto& c : n->children_of("struct-body"))
                    {
                        assert(c != nullptr);
                        if (c->child_count() == 2)
                        {
                            auto decl = c->child(0);
                            assert(decl != nullptr);
                            assert(decl->type_name == "value-declaration");
                            assert(decl->child_count() == 2);
                            res.push_back(parameter::non_ref(
                                scope::normalized_type::of(decl->child(0)),
                                scope::variable_name::of(decl->child(1))));
                        }
                    }
                    return res;
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() >= 5);
                    std::string struct_name = scope::type_name::of(n->child(1));
                    auto members = parse_struct_body(n);
                    return scope::current()->structs().define(
                        struct_name,
                        std::move(members),
                        [&o](const std::string& type, uint32_t size) {
                            assert(builders::of_type(type, size).to(o));
                        });
                }

                static bool copy(const std::vector<std::string>& sources,
                                 const std::string& target,
                                 const std::function<std::string(const std::string&)>& target_naming,
                                 logic_writer& o)
                {
                    assert(!target.empty());
                    assert(target_naming != nullptr);
                    struct_def vs;
                    if (!scope::current()->structs_resolve_variable(target, vs))
                    {
                        return false;
                    }
                    if (vs.primitive_count() != sources.size())
                    {
                        raise_error("Sources are not consistent with the type of ", target);
                        return false;
                    }
                    const auto& prims = vs.primitives();
                    for (size_t i = 0; i < prims.size(); ++i)
                    {
                        if (!builders::of_copy(target_naming(prims[i].name), sources[i]).to(o))
                        {
                            return false;
                        }
                    }
                    return true;
                }

                static bool copy(const std::vector<std::string>& sources, const std::string& target, logic_writer& o)
                {
                    return copy(sources, target, [](const std::string& n) -> std::string { return n; }, o);
                }

                static bool copy(const std::vector<std::string>& sources, const std::string& target, const std::string& indexstr, logic_writer& o)
                {
                    return copy(sources, target, [&indexstr](const std::string& n) -> std::string {
                        return logic::variable::name_of(n, indexstr);
                    }, o);
                }

                static bool pack(const std::vector<std::string>& sources, const std::string& target, logic_writer& o)
                {
                    assert(!target.empty());
                    for (const auto& s : sources)
                    {
                        assert(!s.empty());
                        if (!builders::of_append_slice(target, s).to(o)) return false;
                    }
                    return true;
                }

                static bool unpack(const std::string& source, const std::vector<std::string>& targets, logic_writer& o);

                static bool define_in_vars(const std::string& type, const std::string& name, const struct_def& v)
                {
                    assert(!type.empty() && !name.empty());
                    std::vector<parameter> all;
                    all.push_back(struct_def::nested(type, name));
                    for (const auto& n : v.nesteds()) all.push_back(n);
                    for (const auto& s : all)
                    {
                        assert(!s.ref);
                        if (!scope::current()->variables().define(s.non_ref_type(), s.name))
                        {
                            return false;
                        }
                    }
                    return true;
                }

                static void forward_in_stack(const std::shared_ptr<automata::typed_node>& type_node,
                                             const std::shared_ptr<automata::typed_node>& name_node)
                {
                    assert(type_node != nullptr && name_node != nullptr);
                    std::string type = scope::normalized_type::of(type_node);
                    std::string name = scope::variable_name::of(name_node);
                    struct_def v;
                    if (scope::current()->structs_resolve(type, name, v))
                    {
                        define_in_vars(type, name, v);
                    }
                }

                static bool define_in_stack(const std::shared_ptr<automata::typed_node>& type_node,
                                            const std::shared_ptr<automata::typed_node>& name_node,
                                            logic_writer& o);

                static bool define_in_heap(const std::shared_ptr<automata::typed_node>& type_node,
                                           const std::shared_ptr<automata::typed_node>& name_node,
                                           const std::shared_ptr<automata::typed_node>& length,
                                           logic_writer& o);

                static bool dealloc_from_heap(const std::string& name, logic_writer& o)
                {
                    struct_def v;
                    if (!scope::current()->structs_resolve_variable(name, v))
                    {
                        return false;
                    }
                    return v.for_each_primitive([&o](const parameter& m) {
                        return builders::of_dealloc_heap(m.name).to(o);
                    });
                }

                static bool undefine(const std::string& name, logic_writer& o)
                {
                    struct_def v;
                    if (!scope::current()->structs_resolve_variable(name, v))
                    {
                        return false;
                    }
                    if (!scope::current()->variables_undefine(name)) return false;
                    return v.for_each_primitive([&o](const parameter& m) {
                        return scope::current()->variables_undefine(m.name) &&
                               builders::of_undefine(m.name).to(o);
                    });
                }

                static bool redefine(const std::string& name, const std::string& type, logic_writer& o)
                {
                    struct_def v;
                    if (!scope::current()->structs_resolve(type, name, v))
                    {
                        return false;
                    }
                    if (!scope::current()->variables_redefine(type, name)) return false;
                    return v.for_each_primitive([&o](const parameter& m) {
                        return scope::current()->variables_redefine(m.non_ref_type(), m.name) &&
                               builders::of_redefine(m.name, m.non_ref_type()).to(o);
                    });
                }
            };

            class raw_variable_name : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "raw-variable-name"; }

                static bool build(const std::string& name,
                                  const std::function<bool(const std::string&, const std::vector<parameter>&)>& handle)
                {
                    assert(!name.empty());
                    assert(handle != nullptr);
                    parameter p;
                    if (!scope::current()->variables_resolve(name, p))
                    {
                        return false;
                    }
                    struct_def ps;
                    if (!scope::current()->structs_resolve(p.full_type(), p.name, ps))
                    {
                        ps = struct_def::of_primitive(p.full_type(), p.name);
                    }
                    return handle(p.full_type(), ps.primitives());
                }

                static bool build(const std::string& name)
                {
                    return build(name, [](const std::string& type, const std::vector<parameter>& ps) {
                        scope::current()->value_target().with_value(type, ps);
                        return true;
                    });
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer&) override
                {
                    assert(n != nullptr && n->child_count() == 1);
                    return build(scope::variable_name::of(n->child(0)));
                }
            };

            class name_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "name"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer&) override
                {
                    assert(n != nullptr);
                    return raw_variable_name::build(scope::variable_name::of(n));
                }
            };

            class heap_name : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "heap-name"; }

                static bool build(const std::shared_ptr<automata::typed_node>& index,
                                  logic_writer& o,
                                  const std::function<bool(const std::string&)>& f)
                {
                    assert(index != nullptr && f != nullptr);
                    if (!code_gen_of(index).build(o))
                    {
                        return false;
                    }
                    std::string indexstr;
                    {
                        auto read_target = scope::current()->value_target().primitive_type();
                        if (!read_target.retrieve(indexstr))
                        {
                            raise_error("Index or length of a heap declaration cannot be a struct.");
                            return false;
                        }
                    }
                    return f(indexstr);
                }

                static bool as_raw_variable_name(const std::shared_ptr<automata::typed_node>& index,
                                                 const std::string& name,
                                                 logic_writer& o)
                {
                    assert(index != nullptr && !name.empty());
                    return build(index, o, [&name](const std::string& indexstr) {
                        return raw_variable_name::build(name, [&indexstr](const std::string& type, const std::vector<parameter>& ps) {
                            std::vector<parameter> mapped;
                            mapped.reserve(ps.size());
                            for (const auto& d : ps)
                            {
                                mapped.push_back(d.map_name([&indexstr](const std::string& n) {
                                    return logic::variable::name_of(n, indexstr);
                                }));
                            }
                            scope::current()->value_target().with_value(type, mapped);
                            return true;
                        });
                    });
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && n->child_count() == 4);
                    return as_raw_variable_name(n->child(2), scope::variable_name::of(n->child(0)), o);
                }
            };

            class heap_struct_name_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "heap-struct-name"; }

                static std::pair<std::string, std::shared_ptr<automata::typed_node>> logic_order(
                    const std::shared_ptr<automata::typed_node>& n)
                {
                    assert(n != nullptr && n->child_count() == 3);
                    assert(n->child(0)->child_count() == 4);
                    std::string base = n->child(0)->child(0)->input_without_ignored();
                    std::string dot = n->child(1)->input_without_ignored();
                    std::string field = n->child(2)->input_without_ignored();
                    return {base + dot + field, n->child(0)->child(2)};
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    auto t = logic_order(n);
                    return heap_name::as_raw_variable_name(t.second, t.first, o);
                }
            };

            class value_list : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "value-list"; }

                static void with_empty()
                {
                    scope::current()->value_target().with_value_list({});
                }

                static read_scoped<std::vector<std::string>>::ref current_targets()
                {
                    return scope::current()->value_target().value_list();
                }

                static bool build(const std::function<void(const std::function<void(const std::shared_ptr<automata::typed_node>&)>&)>& foreach_fn,
                                  logic_writer& o)
                {
                    assert(foreach_fn != nullptr);
                    std::vector<std::string> v;
                    bool ok = true;
                    foreach_fn([&](const std::shared_ptr<automata::typed_node>& child) {
                        if (!ok) return;
                        if (!code_gen_of(child).build(o))
                        {
                            ok = false;
                            return;
                        }
                        auto r = scope::current()->value_target().value();
                        v.insert(v.end(), r->names.begin(), r->names.end());
                    });
                    if (!ok) return false;
                    scope::current()->value_target().with_value_list(std::move(v));
                    return true;
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && n->child_count() > 0);
                    return build([&n](const std::function<void(const std::shared_ptr<automata::typed_node>&)>& a) {
                        for (uint32_t i = 0; i < n->child_count(); ++i)
                        {
                            a(n->child(i));
                        }
                    }, o);
                }
            };

            class value_declaration : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "value-declaration"; }

                static bool declare_primitive_type(const std::string& type, const std::string& name, logic_writer& o)
                {
                    if (!scope::current()->structs_is_type_defined(type) &&
                        scope::current()->variables().define(type, name) &&
                        builders::of_define(scope::fully_qualified_variable_name::of(name),
                                           scope::normalized_type::of(type)).to(o))
                    {
                        return true;
                    }
                    raise_error("Failed to declare ", type, " with name ", name, " as a primitive type variable.");
                    return false;
                }

                static bool build(const std::shared_ptr<automata::typed_node>& n,
                                  bool class_construct,
                                  logic_writer& o);

                static bool without_class_construct(const std::shared_ptr<automata::typed_node>& n, logic_writer& o)
                {
                    return build(n, false, o);
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    return build(n, true, o);
                }
            };

            class heap_declaration : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "heap-declaration"; }

                static bool declare_primitive_type(std::string type,
                                                   const std::string& name,
                                                   const std::string& length,
                                                   logic_writer& o)
                {
                    type = scope::normalized_type::parameter_type_of(type).full_type();
                    assert(!scope::current()->structs_is_type_defined(type));
                    return scope::current()->variables().define(type, name) &&
                           builders::of_define_heap(name, type, length).to(o);
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && n->child_count() == 2);
                    assert(n->child(1)->child_count() == 4);
                    auto type = n->child(0);
                    auto name = n->child(1)->child(0);
                    auto length = n->child(1)->child(2);
                    assert(type != nullptr && name != nullptr && length != nullptr);
                    return struct_node::define_in_heap(type, name, length, o) ||
                           heap_name::build(length, o, [&type, &name, &o](const std::string& len_name) {
                               return declare_primitive_type(scope::normalized_type::of(type),
                                                             scope::fully_qualified_variable_name::of(name),
                                                             len_name,
                                                             o);
                           });
                }
            };

            inline bool struct_node::unpack(const std::string& source, const std::vector<std::string>& targets, logic_writer& o)
            {
                assert(!source.empty());
                if (targets.empty()) return true;
                std::string index = targets[0] + "@index";
                assert(value_declaration::declare_primitive_type(integer_node::type_name, index, o));
                for (const auto& target : targets)
                {
                    assert(!target.empty());
                    if (!builders::of_cut_slice(target, source, index).to(o) ||
                        !builders::of_add(index, index, "@@prefixes@constants@int_1").to(o))
                    {
                        return false;
                    }
                }
                return true;
            }

            inline bool struct_node::define_in_stack(const std::shared_ptr<automata::typed_node>& type_node,
                                                     const std::shared_ptr<automata::typed_node>& name_node,
                                                     logic_writer& o)
            {
                assert(type_node != nullptr && name_node != nullptr);
                std::string type = scope::normalized_type::of(type_node);
                std::string name = scope::variable_name::of(name_node);
                struct_def v;
                if (!scope::current()->structs_resolve(type, name, v) || !define_in_vars(type, name, v))
                {
                    return false;
                }
                return v.for_each_primitive([&o](const parameter& m) {
                    return value_declaration::declare_primitive_type(m.non_ref_type(), m.name, o);
                });
            }

            inline bool struct_node::define_in_heap(const std::shared_ptr<automata::typed_node>& type_node,
                                                    const std::shared_ptr<automata::typed_node>& name_node,
                                                    const std::shared_ptr<automata::typed_node>& length,
                                                    logic_writer& o)
            {
                assert(type_node != nullptr && name_node != nullptr && length != nullptr);
                std::string type = scope::normalized_type::of(type_node);
                std::string name = scope::variable_name::of(name_node);
                struct_def v;
                if (!scope::current()->structs_resolve(type, name, v) || !define_in_vars(type, name, v))
                {
                    return false;
                }
                return heap_name::build(length, o, [&v, &o](const std::string& len_name) {
                    return v.for_each_primitive([&len_name, &o](const parameter& m) {
                        return heap_declaration::declare_primitive_type(m.non_ref_type(), m.name, len_name, o);
                    });
                });
            }

            class value_clause : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "value-clause"; }

            private:
                static bool build_assignment(const std::string& name,
                                             const std::shared_ptr<automata::typed_node>& value,
                                             const std::function<bool(const std::string&, const std::vector<std::string>&)>& struct_copy,
                                             const std::function<bool(const std::string&, const std::string&)>& primitive_copy,
                                             logic_writer& o)
                {
                    assert(!name.empty() && value != nullptr && struct_copy != nullptr && primitive_copy != nullptr);
                    parameter p;
                    function_signature<parameter_type> delegate_definition;
                    if (!scope::current()->variables_resolve(name, p, &delegate_definition))
                    {
                        return false;
                    }
                    bool is_delegate = scope::current()->delegates_retrieve(p.full_type(), delegate_definition);
                    if (is_delegate)
                    {
                        std::string target_function_name = logic_name::of_function(
                            scope::fully_qualified_function_name::of(value),
                            delegate_definition.parameters);
                        if (scope::current()->functions().is_defined(target_function_name))
                        {
                            scope::current()->call_hierarchy().to(target_function_name);
                            return builders::of_address_of(name, target_function_name).to(o);
                        }
                        return builders::of_copy(name, scope::function_name::of(value)).to(o);
                    }
                    if (!code_gen_of(value).build(o))
                    {
                        return false;
                    }
                    if (scope::current()->structs_is_type_defined(p.full_type()))
                    {
                        auto r = scope::current()->value_target().value();
                        if (r->type != p.full_type())
                        {
                            raise_error("Type ", p.full_type(), " of ", name, " does not match rvalue ", r->type);
                            return false;
                        }
                        return struct_copy(p.name, r->names);
                    }
                    std::string s;
                    {
                        auto r = scope::current()->value_target().primitive_type();
                        if (!r.retrieve(s))
                        {
                            raise_error("Failed to retrieve a primitive-type target from the r-value, received a struct?");
                            return false;
                        }
                    }
                    return primitive_copy(p.name, s);
                }

                static bool build_assignment(const std::shared_ptr<automata::typed_node>& name_node,
                                             const std::shared_ptr<automata::typed_node>& value,
                                             const std::function<bool(const std::string&, const std::vector<std::string>&)>& struct_copy,
                                             const std::function<bool(const std::string&, const std::string&)>& primitive_copy,
                                             logic_writer& o)
                {
                    return build_assignment(scope::fully_qualified_variable_name::of(name_node), value, struct_copy, primitive_copy, o);
                }

            public:
                static bool stack_name_build(const std::shared_ptr<automata::typed_node>& name,
                                             const std::shared_ptr<automata::typed_node>& value,
                                             logic_writer& o)
                {
                    assert(name != nullptr && name->type_name == "name");
                    return build_assignment(name,
                                            value,
                                            [&o](const std::string& n, const std::vector<std::string>& r) {
                                                return struct_node::copy(r, n, o);
                                            },
                                            [&o](const std::string& n, const std::string& r) {
                                                return builders::of_copy(n, r).to(o);
                                            },
                                            o);
                }

                static bool heap_name_build(const std::shared_ptr<automata::typed_node>& index_node,
                                            const std::string& name,
                                            const std::shared_ptr<automata::typed_node>& value,
                                            logic_writer& o)
                {
                    assert(index_node != nullptr && !name.empty() && value != nullptr);
                    return heap_name::build(index_node, o, [&name, &value, &o](const std::string& indexstr) {
                        return build_assignment(name,
                                                value,
                                                [&indexstr, &o](const std::string& n2, const std::vector<std::string>& r) {
                                                    return struct_node::copy(r, n2, indexstr, o);
                                                },
                                                [&indexstr, &o](const std::string& n2, const std::string& r) {
                                                    return builders::of_copy(logic::variable::name_of(n2, indexstr), r).to(o);
                                                },
                                                o);
                    });
                }

                static bool heap_name_build(const std::shared_ptr<automata::typed_node>& name,
                                            const std::shared_ptr<automata::typed_node>& value,
                                            logic_writer& o)
                {
                    assert(name != nullptr && name->type_name == "heap-name");
                    return heap_name_build(name->child(2), scope::fully_qualified_variable_name::of(name->child(0)), value, o);
                }

                static bool build(const std::shared_ptr<automata::typed_node>& name,
                                  const std::shared_ptr<automata::typed_node>& value,
                                  logic_writer& o)
                {
                    assert(name != nullptr && value != nullptr);
                    if (name->type_name == "variable-name")
                    {
                        return stack_name_build(name->child(0), value, o);
                    }
                    if (name->type_name == "name")
                    {
                        return stack_name_build(name, value, o);
                    }
                    if (name->type_name == "heap-name")
                    {
                        return heap_name_build(name, value, o);
                    }
                    if (name->type_name == "heap-struct-name")
                    {
                        auto t = heap_struct_name_node::logic_order(name);
                        return heap_name_build(t.second, t.first, value, o);
                    }
                    assert(false);
                    return false;
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && n->child_count() == 3);
                    auto name = n->child(0)->child(0);
                    auto value = n->child(2);
                    return build(name, value, o);
                }
            };
        }
    }
}
