#pragma once
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <unordered_set>
#include <algorithm>
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../../utils/strutils.hpp"
#include "../typed_word.hpp"
#include "../typed_node.hpp"
#include "../syntax_collection.hpp"
#include "matching.hpp"
#include "matching_group.hpp"
#include "syntax.hpp"
#include "rule.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class syntaxer
            {
            private:
                syntax_collection collection_;
                std::vector<uint32_t> root_types_;
                std::unordered_set<uint32_t> ignore_types_;
                std::shared_ptr<matching_group> mg_;

                static std::string debug_str_token(const std::string& input, const std::string& type_name)
                {
                    return strcat(input, "(", input.size(), "):[", type_name, "]");
                }

                std::string debug_str(const std::vector<typed_word>& v, uint32_t p) const
                {
                    uint32_t start = p >= 6 ? p - 6 : 0;
                    uint32_t end = std::min(start + 13, static_cast<uint32_t>(v.size()));
                    std::string r;
                    for (uint32_t i = start; i < end; ++i)
                    {
                        if (i == p)
                        {
                            r += ">>> ";
                        }
                        r += debug_str_token(v[i].str(), collection_.type_name(v[i].type));
                        r += ' ';
                        if (i == p)
                        {
                            r += " <<< ";
                        }
                    }
                    return r;
                }

                void log_unmatched(const std::vector<typed_word>& v, uint32_t p, const matching::result& f) const
                {
                    auto format_pos = [&](uint32_t pos) -> std::string {
                        std::string tok = (pos < v.size()) ? v[pos].str() : "{END-OF-INPUT}";
                        return strcat(tok, " @ ", pos, " - ", debug_str(v, pos));
                    };
                    uint32_t longest_pos = f.fal.pos;
                    raise_error("[syntaxer] Cannot match token ", format_pos(p), ". Longest match ", format_pos(longest_pos));
                }

            public:
                syntaxer(syntax_collection collection,
                         std::unordered_set<uint32_t> ignore_types,
                         std::vector<uint32_t> root_types)
                    : collection_(std::move(collection)),
                      root_types_(std::move(root_types)),
                      ignore_types_(std::move(ignore_types))
                {
                    assert(collection_.complete());
                    assert(!root_types_.empty());
                    std::vector<std::shared_ptr<matching>> root_matchings;
                    root_matchings.reserve(root_types_.size());
                    for (uint32_t t : root_types_)
                    {
                        std::shared_ptr<syntax> s;
                        assert(collection_.get(t, s));
                        root_matchings.push_back(s);
                    }
                    mg_ = std::make_shared<matching_group>(collection_, std::move(root_matchings));
                }

                std::optional<std::shared_ptr<typed_node>> match(const std::vector<typed_word>& raw_v) const
                {
                    assert(!root_types_.empty());
                    std::vector<typed_word> filtered;
                    filtered.reserve(raw_v.size());
                    for (const auto& t : raw_v)
                    {
                        if (ignore_types_.find(t.type) == ignore_types_.end())
                        {
                            filtered.push_back(t);
                        }
                    }
                    if (filtered.empty())
                    {
                        return std::nullopt;
                    }

                    auto v = std::make_shared<const std::vector<typed_word>>(std::move(filtered));
                    auto root = typed_node::of_root(v);
                    uint32_t p = 0;
                    while (p < v->size())
                    {
                        matching::result m = mg_->match(v, p);
                        if (m.failed())
                        {
                            log_unmatched(*v, p, m);
                            return std::nullopt;
                        }
                        assert(m.suc.has_value());
                        if (p == m.suc->pos)
                        {
                            return std::nullopt;
                        }
                        assert(m.suc->pos <= v->size());
                        root->attach(m.suc->nodes);
                        p = m.suc->pos;
                    }
                    assert(p == v->size());
                    assert(!root->leaf());
                    return root;
                }

                bool match(const std::vector<typed_word>& v, std::shared_ptr<typed_node>& root) const
                {
                    auto r = match(v);
                    if (r.has_value())
                    {
                        root = *r;
                        return true;
                    }
                    return false;
                }

                const syntax_collection& collection() const { return collection_; }
                const std::vector<uint32_t>& root_types() const { return root_types_; }
                const std::unordered_set<uint32_t>& ignore_types() const { return ignore_types_; }
            };

            class rule::exporter
            {
            public:
                std::shared_ptr<class syntaxer> syn;
                std::unordered_set<uint32_t> ignore_types;
                std::vector<uint32_t> root_types;
                syntax_collection collection;

                explicit exporter(rule& r)
                    : ignore_types(r.ignores),
                      root_types(r.roots),
                      collection(r.collection)
                {
                    syn = std::make_shared<class syntaxer>(collection, ignore_types, root_types);
                }

                std::shared_ptr<class syntaxer> syntaxer() const { return syn; }
            };

            inline rule::exporter rule::export_syntaxer()
            {
                return exporter(*this);
            }
        }

        using syntaxer = syntax::syntaxer;
    }
}
