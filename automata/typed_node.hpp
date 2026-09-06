#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <cstdint>
#include <algorithm>
#include "../app_info/assert.hpp"
#include "../utils/strutils.hpp"
#include "typed_word.hpp"

namespace osi
{
    namespace automata
    {
        class typed_node : public std::enable_shared_from_this<typed_node>
        {
        public:
            constexpr static uint32_t root_type = 0;
            inline static const std::string root_type_name = "ROOT";

            uint32_t type{root_type};
            std::string type_name{root_type_name};
            uint32_t word_start{0};
            uint32_t word_end{0}; // exclusive
            std::vector<std::shared_ptr<typed_node>> subnodes;
            std::shared_ptr<const std::vector<typed_word>> ref;
            typed_node* parent{nullptr};

            struct stop_navigating_sub_nodes_exception : public std::exception {};

            typed_node(std::shared_ptr<const std::vector<typed_word>> ref_words,
                       uint32_t type_id,
                       std::string type_name_str,
                       uint32_t word_start_idx,
                       uint32_t word_end_idx)
                : type(type_id),
                  type_name(std::move(type_name_str)),
                  word_start(word_start_idx),
                  word_end(word_end_idx),
                  ref(std::move(ref_words))
            {
                assert(ref != nullptr);
                assert(word_start <= word_end);
                assert(word_end <= ref->size());
                assert(!type_name.empty());
            }

            static std::shared_ptr<typed_node> of_root(std::shared_ptr<const std::vector<typed_word>> ref_words)
            {
                assert(ref_words != nullptr);
                return std::make_shared<typed_node>(ref_words, root_type, root_type_name, 0, static_cast<uint32_t>(ref_words->size()));
            }

            static std::shared_ptr<typed_node> of_root(const std::vector<typed_word>& ref_words)
            {
                auto ptr = std::make_shared<std::vector<typed_word>>(ref_words);
                return of_root(ptr);
            }

            void attach_to(const std::shared_ptr<typed_node>& p)
            {
                assert(p != nullptr);
                assert(this->parent == nullptr);
                this->parent = p.get();
                p->subnodes.push_back(shared_from_this());
            }

            void attach(const std::vector<std::shared_ptr<typed_node>>& nodes)
            {
                for (const auto& n : nodes)
                {
                    assert(n != nullptr);
                    n->parent = this;
                    this->subnodes.push_back(n);
                }
            }

            bool root() const
            {
                return parent == nullptr;
            }

            uint32_t child_index(const std::shared_ptr<typed_node>& c) const
            {
                assert(c != nullptr);
                assert(c->parent == this);
                assert(!subnodes.empty());
                for (size_t i = 0; i < subnodes.size(); ++i)
                {
                    if (subnodes[i] == c)
                    {
                        return static_cast<uint32_t>(i);
                    }
                }
                assert(false);
                return UINT32_MAX;
            }

            std::shared_ptr<typed_node> child(uint32_t id) const
            {
                assert(id < subnodes.size(), type_name);
                return subnodes[id];
            }

            std::shared_ptr<typed_node> child() const
            {
                assert(child_count() == 1, type_name);
                return child(0);
            }

            static std::shared_ptr<typed_node> child(const std::shared_ptr<typed_node>& n)
            {
                assert(n != nullptr);
                return n->child();
            }

            std::shared_ptr<typed_node> last_child() const
            {
                assert(child_count() > 0, type_name);
                return child(child_count() - 1);
            }

            uint32_t child_count() const
            {
                return static_cast<uint32_t>(subnodes.size());
            }

            std::vector<std::shared_ptr<typed_node>> children_of(const std::string& name) const
            {
                assert(!leaf());
                assert(!name.empty());
                std::vector<std::shared_ptr<typed_node>> r;
                for (uint32_t i = 0; i < child_count(); ++i)
                {
                    if (child(i)->type_name == name)
                    {
                        r.push_back(child(i));
                    }
                }
                return r;
            }

            bool leaf() const
            {
                return subnodes.empty();
            }

            const typed_word& word(uint32_t id) const
            {
                assert(ref != nullptr);
                assert(id < word_count());
                return (*ref)[word_start + id];
            }

            const typed_word& word() const
            {
                assert(word_count() == 1);
                return word(0);
            }

            uint32_t word_count() const
            {
                return word_end - word_start;
            }

            uint32_t char_start() const
            {
                return word(0).start;
            }

            uint32_t char_end() const
            {
                return word(word_count() - 1).end;
            }

            bool only_descendant(std::shared_ptr<typed_node>& o)
            {
                std::shared_ptr<typed_node> n = shared_from_this();
                while (true)
                {
                    if (n->leaf())
                    {
                        o = n;
                        return true;
                    }
                    if (n->child_count() > 1)
                    {
                        return false;
                    }
                    n = n->child();
                }
            }

            std::shared_ptr<typed_node> only_descendant()
            {
                std::shared_ptr<typed_node> r;
                assert(only_descendant(r));
                return r;
            }

            bool immediate_descendant_of(const std::string& name) const
            {
                return parent != nullptr && parent->type_name == name;
            }

            bool immediate_descentdant_of(const std::string& name) const
            {
                return immediate_descendant_of(name);
            }

            bool immediate_descendant_of(const std::vector<std::string>& names) const
            {
                assert(!names.empty());
                const typed_node* n = this;
                for (size_t i = 0; i < names.size(); ++i)
                {
                    n = n->parent;
                    if (n == nullptr) return false;
                    if (n->type_name != names[i]) return false;
                }
                return true;
            }

            bool immediate_descentdant_of(const std::vector<std::string>& names) const
            {
                return immediate_descendant_of(names);
            }

            bool descendant_of(const std::vector<std::string>& names) const
            {
                assert(!names.empty());
                const typed_node* n = this;
                size_t i = 0;
                while (true)
                {
                    if (n->root()) return false;
                    n = n->parent;
                    assert(n != nullptr);
                    if (n->type_name == names[i])
                    {
                        i++;
                        if (i == names.size()) return true;
                    }
                }
            }

            bool descentdant_of(const std::vector<std::string>& names) const
            {
                return descendant_of(names);
            }

            bool ancestor_of(const std::string& name, typed_node*& o)
            {
                assert(!name.empty());
                typed_node* n = this;
                while (true)
                {
                    assert(n != nullptr);
                    if (n->type_name == name)
                    {
                        o = n;
                        return true;
                    }
                    if (n->root()) return false;
                    n = n->parent;
                }
            }

            bool ancestor_of(const std::string& name, const typed_node*& o) const
            {
                assert(!name.empty());
                const typed_node* n = this;
                while (true)
                {
                    assert(n != nullptr);
                    if (n->type_name == name)
                    {
                        o = n;
                        return true;
                    }
                    if (n->root()) return false;
                    n = n->parent;
                }
            }

            typed_node* ancestor_of(const std::string& name)
            {
                typed_node* o = nullptr;
                assert(ancestor_of(name, o));
                return o;
            }

            const typed_node* ancestor_of(const std::string& name) const
            {
                const typed_node* o = nullptr;
                assert(ancestor_of(name, o));
                return o;
            }

            class child_named_map
            {
            private:
                std::unordered_map<std::string, std::vector<std::shared_ptr<typed_node>>> m_;

            public:
                explicit child_named_map(const typed_node* n)
                {
                    assert(n != nullptr);
                    for (uint32_t i = 0; i < n->child_count(); ++i)
                    {
                        m_[n->child(i)->type_name].push_back(n->child(i));
                    }
                }

                bool nodes(const std::string& name, std::vector<std::shared_ptr<typed_node>>& o) const
                {
                    auto it = m_.find(name);
                    if (it == m_.end()) return false;
                    o = it->second;
                    return true;
                }

                bool node(const std::string& name, std::shared_ptr<typed_node>& o) const
                {
                    auto it = m_.find(name);
                    if (it == m_.end() || it->second.size() != 1) return false;
                    o = it->second[0];
                    return true;
                }

                std::vector<std::shared_ptr<typed_node>> nodes(const std::string& name) const
                {
                    std::vector<std::shared_ptr<typed_node>> o;
                    assert(nodes(name, o));
                    return o;
                }

                std::shared_ptr<typed_node> node(const std::string& name) const
                {
                    std::shared_ptr<typed_node> o;
                    assert(node(name, o));
                    return o;
                }
            };

            child_named_map named_children() const
            {
                return child_named_map(this);
            }

            std::string debug_str() const
            {
                std::string s;
                debug_str_internal(s);
                return s;
            }

            bool only_descendant_str(std::string& o)
            {
                std::shared_ptr<typed_node> n;
                if (!only_descendant(n)) return false;
                o = n->word().str();
                return true;
            }

            std::string only_descendant_str()
            {
                std::string r;
                assert(only_descendant_str(r));
                return r;
            }

            std::string input(const std::string& space = " ") const
            {
                std::string s;
                for (uint32_t i = 0; i < word_count(); ++i)
                {
                    s += word(i).str();
                    if (i + 1 < word_count())
                    {
                        s += space;
                    }
                }
                return s;
            }

            std::string input_without_ignored() const
            {
                return input("");
            }

            std::string trace_back_str() const
            {
                std::string s;
                const typed_node* n = this;
                while (n != nullptr)
                {
                    s += "@" + n->type_name + ": ";
                    uint32_t limit = std::min(n->word_count(), 3u);
                    for (uint32_t i = 0; i < limit; ++i)
                    {
                        s += n->word(i).str() + " ";
                    }
                    s += "\n";
                    if (n->root()) break;
                    n = n->parent;
                }
                return s;
            }

            void dfs(const std::function<void(const std::shared_ptr<typed_node>&, const std::function<void()>&)>& node_handle,
                     const std::function<void(const std::shared_ptr<typed_node>&)>& leaf_handle)
            {
                assert(node_handle != nullptr);
                assert(leaf_handle != nullptr);

                if (leaf())
                {
                    leaf_handle(shared_from_this());
                    return;
                }

                try
                {
                    node_handle(shared_from_this(), []() { throw stop_navigating_sub_nodes_exception(); });
                }
                catch (const stop_navigating_sub_nodes_exception&)
                {
                    return;
                }

                assert(child_count() > 0);
                for (const auto& c : subnodes)
                {
                    c->dfs(node_handle, leaf_handle);
                }
            }

        private:
            void nodes_debug_str(std::string& s) const
            {
                if (subnodes.empty()) return;
                s += "{";
                for (size_t i = 0; i < subnodes.size(); ++i)
                {
                    if (i > 0) s += ", ";
                    subnodes[i]->debug_str_internal(s);
                }
                s += "}";
            }

            void debug_str_internal(std::string& s) const
            {
                s += "[" + std::to_string(type) + ":" + type_name + "<" +
                     std::to_string(word_start) + "," + std::to_string(word_end) + ">";
                nodes_debug_str(s);
                s += "]";
            }
        };
    }
}
