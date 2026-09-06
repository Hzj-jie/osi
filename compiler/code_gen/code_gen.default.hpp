#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "code_gen.hpp"
#include "../../app_info/assert.hpp"
#include "../../automata/typed_node.hpp"

namespace osi
{
    namespace compiler
    {
        template <typename Writer>
        class code_gen_delegate : public code_gen<Writer>
        {
        public:
            using simple_func_t = std::function<bool(const std::shared_ptr<automata::typed_node>&, Writer&)>;
            using full_func_t = std::function<bool(code_gens<Writer>&, const std::shared_ptr<automata::typed_node>&, Writer&)>;

        private:
            full_func_t f_;

        public:
            explicit code_gen_delegate(simple_func_t f)
                : f_([f = std::move(f)](code_gens<Writer>&, const std::shared_ptr<automata::typed_node>& n, Writer& o) {
                    return f(n, o);
                })
            {
                assert(f_ != nullptr);
            }

            explicit code_gen_delegate(full_func_t f)
                : f_(std::move(f))
            {
                assert(f_ != nullptr);
            }

            bool build(const std::shared_ptr<automata::typed_node>& n, Writer& o) override
            {
                assert(n != nullptr);
                // Call full_func_t with a dummy code_gens if invoked standalone
                static code_gens<Writer> dummy;
                return f_(dummy, n, o);
            }

            static std::function<void(code_gens<Writer>&)> of(
                const std::string& name,
                simple_func_t f)
            {
                return [name, f = std::move(f)](code_gens<Writer>& b) {
                    b.register_gen(name, std::make_shared<code_gen_delegate<Writer>>(f));
                };
            }

            static std::function<void(code_gens<Writer>&)> of(
                const std::string& name,
                full_func_t f)
            {
                return [name, f = std::move(f)](code_gens<Writer>& b) {
                    b.register_gen(name, std::make_shared<code_gen_delegate<Writer>>(
                        [&b, f](code_gens<Writer>&, const std::shared_ptr<automata::typed_node>& n, Writer& o) {
                            return f(b, n, o);
                        }));
                };
            }
        };

        struct code_gen_default
        {
            template <typename Writer>
            static std::function<void(code_gens<Writer>&)> of_ignore(const std::string& name)
            {
                return code_gen_delegate<Writer>::of(
                    name,
                    [](code_gens<Writer>&, const std::shared_ptr<automata::typed_node>&, Writer&) {
                        return true;
                    });
            }

            template <typename Writer>
            static std::function<void(code_gens<Writer>&)> of_children(
                const std::string& name,
                const std::vector<uint32_t>& selected_children)
            {
                assert(!selected_children.empty());
                return code_gen_delegate<Writer>::of(
                    name,
                    [selected_children](code_gens<Writer>& this_cg,
                                        const std::shared_ptr<automata::typed_node>& n,
                                        Writer& o) {
                        assert(n != nullptr);
                        for (uint32_t idx : selected_children)
                        {
                            assert(n->child_count() > idx);
                            if (!this_cg.of(n->child(idx)).build(o))
                            {
                                return false;
                            }
                        }
                        return true;
                    });
            }

            template <typename Writer>
            static std::function<void(code_gens<Writer>&)> of_first_child(const std::string& name)
            {
                return of_children<Writer>(name, {0});
            }

            template <typename Writer>
            static std::function<void(code_gens<Writer>&)> of_only_child(const std::string& name)
            {
                return code_gen_delegate<Writer>::of(
                    name,
                    [](code_gens<Writer>& this_cg,
                       const std::shared_ptr<automata::typed_node>& n,
                       Writer& o) {
                        assert(n != nullptr);
                        assert(n->child_count() == 1);
                        return this_cg.of(n->child()).build(o);
                    });
            }

            template <typename Writer>
            static std::function<void(code_gens<Writer>&)> of_ignore_last_child(const std::string& name)
            {
                return code_gen_delegate<Writer>::of(
                    name,
                    [](code_gens<Writer>& this_cg,
                       const std::shared_ptr<automata::typed_node>& n,
                       Writer& o) {
                        assert(n != nullptr);
                        if (n->child_count() <= 1)
                        {
                            return true;
                        }
                        for (uint32_t i = 0; i < n->child_count() - 1; ++i)
                        {
                            if (!this_cg.of(n->child(i)).build(o))
                            {
                                return false;
                            }
                        }
                        return true;
                    });
            }

            template <typename Writer>
            static std::function<void(code_gens<Writer>&)> of_all_children(const std::string& name)
            {
                return code_gen_delegate<Writer>::of(
                    name,
                    [](code_gens<Writer>& this_cg,
                       const std::shared_ptr<automata::typed_node>& n,
                       Writer& o) {
                        assert(n != nullptr);
                        return this_cg.of_all_children(n).build(o);
                    });
            }

            template <typename Writer>
            static std::function<void(code_gens<Writer>&)> of_input(const std::string& name)
            {
                return code_gen_delegate<Writer>::of(
                    name,
                    [](const std::shared_ptr<automata::typed_node>& n, Writer& o) {
                        assert(n != nullptr);
                        return o.append(n->input());
                    });
            }

            template <typename Writer>
            static std::function<void(code_gens<Writer>&)> of_input_without_ignored(const std::string& name)
            {
                return code_gen_delegate<Writer>::of(
                    name,
                    [](const std::shared_ptr<automata::typed_node>& n, Writer& o) {
                        assert(n != nullptr);
                        return o.append(n->input_without_ignored());
                    });
            }

            template <typename Writer>
            static std::function<void(code_gens<Writer>&)> of_only_descendant_str(const std::string& name)
            {
                return code_gen_delegate<Writer>::of(
                    name,
                    [](const std::shared_ptr<automata::typed_node>& n, Writer& o) {
                        assert(n != nullptr);
                        return o.append(n->only_descendant_str());
                    });
            }
        };
    }
}
