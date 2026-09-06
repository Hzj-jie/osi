#pragma once
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../../automata/typed_node.hpp"

namespace osi
{
    namespace compiler
    {
        template <typename Writer>
        class code_gen
        {
        public:
            virtual ~code_gen() = default;
            virtual bool build(const std::shared_ptr<automata::typed_node>& n, Writer& o) = 0;
        };

        template <typename Writer>
        class code_gens
        {
        public:
            class code_gen_proxy
            {
            private:
                std::shared_ptr<code_gen<Writer>> b_;
                std::shared_ptr<automata::typed_node> n_;

            public:
                code_gen_proxy(std::shared_ptr<code_gen<Writer>> b, std::shared_ptr<automata::typed_node> n)
                    : b_(std::move(b)), n_(std::move(n))
                {
                    assert(b_ != nullptr);
                    assert(n_ != nullptr);
                }

                bool build(Writer& o) const
                {
                    if (b_->build(n_, o))
                    {
                        return true;
                    }
                    if (n_->child_count() != 1)
                    {
                        raise_error("Failed to build node ", n_->input());
                    }
                    return false;
                }

                bool dump(std::string& o) const
                {
                    Writer w;
                    if (!build(w))
                    {
                        return false;
                    }
                    o = w.ToString();
                    return true;
                }

                std::string dump() const
                {
                    std::string r;
                    assert(dump(r));
                    return r;
                }
            };

            class code_gen_all_children_proxy
            {
            private:
                code_gens<Writer>& l_;
                std::shared_ptr<automata::typed_node> n_;

            public:
                code_gen_all_children_proxy(code_gens<Writer>& l, std::shared_ptr<automata::typed_node> n)
                    : l_(l), n_(std::move(n))
                {
                    assert(n_ != nullptr);
                }

                bool build(Writer& o) const
                {
                    for (uint32_t i = 0; i < n_->child_count(); ++i)
                    {
                        if (!l_.of(n_->child(i)).build(o))
                        {
                            return false;
                        }
                    }
                    return true;
                }

                bool dump(std::vector<std::string>& o) const
                {
                    o.clear();
                    o.reserve(n_->child_count());
                    for (uint32_t i = 0; i < n_->child_count(); ++i)
                    {
                        std::string s;
                        if (!l_.of(n_->child(i)).dump(s))
                        {
                            raise_error("Failed to dump ", n_->child(i)->input());
                            return false;
                        }
                        o.push_back(std::move(s));
                    }
                    return true;
                }

                std::vector<std::string> dump() const
                {
                    std::vector<std::string> r;
                    assert(dump(r));
                    return r;
                }
            };

        private:
            std::unordered_map<std::string, std::shared_ptr<code_gen<Writer>>> m_;

        public:
            code_gens() = default;

            template <typename T>
            static std::string code_gen_name()
            {
                return T::node_name();
            }

            void register_gen(const std::string& name, std::shared_ptr<code_gen<Writer>> gen)
            {
                assert(!name.empty());
                assert(gen != nullptr);
                m_[name] = std::move(gen);
            }

            std::shared_ptr<code_gen<Writer>> code_gen_of(const std::string& name) const
            {
                auto it = m_.find(name);
                if (it != m_.end())
                {
                    return it->second;
                }
                raise_error("Cannot find code_gen of ", name);
                assert(false);
                return nullptr;
            }

            code_gen_proxy of(const std::shared_ptr<automata::typed_node>& n)
            {
                assert(n != nullptr);
                return code_gen_proxy(code_gen_of(n->type_name), n);
            }

            code_gen_all_children_proxy of_all_children(const std::shared_ptr<automata::typed_node>& n)
            {
                assert(n != nullptr);
                return code_gen_all_children_proxy(*this, n);
            }
        };
    }
}
