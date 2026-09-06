#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cctype>
#include <cstdint>
#include "characters.hpp"
#include "matching.hpp"
#include "single_matching.hpp"
#include "empty_matching.hpp"
#include "matching_delegate.hpp"
#include "matching_group.hpp"
#include "optional_matching_group.hpp"
#include "any_matching_group.hpp"
#include "multi_matching_group.hpp"
#include "syntax.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class matching_creator
            {
            private:
                static void consume_space_chars(const std::string& i, uint32_t& pos)
                {
                    while (pos < i.size() && characters::matching_separators.find(i[pos]) != std::string_view::npos)
                    {
                        pos++;
                    }
                }

                static bool create_matching(std::string s, syntax_collection& collection, std::shared_ptr<matching>& o)
                {
                    size_t start = 0;
                    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;
                    size_t end = s.size();
                    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;
                    s = s.substr(start, end - start);

                    if (!characters::valid_type_str(s))
                    {
                        return false;
                    }

                    uint32_t id = 0;
                    if (collection.token_type(s, id))
                    {
                        o = std::make_shared<single_matching>(collection, id);
                    }
                    else
                    {
                        id = collection.define(s);
                        o = std::make_shared<matching_delegate>(collection, id);
                    }
                    assert(o != nullptr);
                    return true;
                }

                static bool create_without_space_chars(const std::string& i,
                                                      syntax_collection& collection,
                                                      uint32_t& pos,
                                                      std::shared_ptr<matching>& o)
                {
                    if (pos >= i.size())
                    {
                        return false;
                    }
                    assert(characters::matching_separators.find(i[pos]) == std::string_view::npos);

                    if (i[pos] == characters::matching_group_start)
                    {
                        size_t e = i.find(characters::matching_group_end, pos + 1);
                        if (e == std::string::npos)
                        {
                            return false;
                        }
                        std::string inner = i.substr(pos + 1, e - pos - 1);
                        std::vector<std::string> ss;
                        size_t sub_start = 0;
                        while (true)
                        {
                            size_t c_pos = inner.find(characters::matching_group_separator, sub_start);
                            if (c_pos == std::string::npos)
                            {
                                ss.push_back(inner.substr(sub_start));
                                break;
                            }
                            ss.push_back(inner.substr(sub_start, c_pos - sub_start));
                            sub_start = c_pos + 1;
                        }
                        if (ss.empty())
                        {
                            return false;
                        }

                        std::vector<std::shared_ptr<matching>> ms;
                        ms.reserve(ss.size());
                        for (const auto& item : ss)
                        {
                            std::shared_ptr<matching> item_m;
                            if (!create_matching(item, collection, item_m))
                            {
                                return false;
                            }
                            ms.push_back(item_m);
                        }
                        o = std::make_shared<matching_group>(collection, std::move(ms));
                        pos = static_cast<uint32_t>(e + 1);
                        while (pos < i.size())
                        {
                            if (i[pos] == characters::optional_matching)
                            {
                                o = std::make_shared<optional_matching_group>(collection, o);
                                pos++;
                            }
                            else if (i[pos] == characters::any_matching)
                            {
                                o = std::make_shared<any_matching_group>(collection, o);
                                pos++;
                            }
                            else if (i[pos] == characters::multi_matching)
                            {
                                o = std::make_shared<multi_matching_group>(collection, o);
                                pos++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        return true;
                    }

                    size_t e = i.find_first_of(" \t\r\n", pos);
                    if (e == std::string::npos)
                    {
                        e = i.size();
                    }
                    if (!create_matching(i.substr(pos, e - pos), collection, o))
                    {
                        return false;
                    }
                    pos = static_cast<uint32_t>(e);
                    return true;
                }

            public:
                static bool create(const std::string& i,
                                   syntax_collection& collection,
                                   uint32_t& pos,
                                   std::shared_ptr<matching>& o)
                {
                    consume_space_chars(i, pos);
                    if (create_without_space_chars(i, collection, pos, o))
                    {
                        consume_space_chars(i, pos);
                        return true;
                    }
                    return false;
                }
            };

            inline bool syntax::create(uint32_t type, const std::string& s, syntax_collection& collection, std::shared_ptr<syntax>& o)
            {
                std::vector<std::shared_ptr<matching>> ms;
                uint32_t pos = 0;
                while (pos < s.size())
                {
                    std::shared_ptr<matching> m;
                    if (!matching_creator::create(s, collection, pos, m))
                    {
                        return false;
                    }
                    ms.push_back(m);
                }
                if (ms.empty())
                {
                    ms.push_back(std::make_shared<empty_matching>(collection));
                }
                o = std::make_shared<syntax>(collection, type, std::move(ms));
                return collection.set(type, o);
            }

            inline bool syntax::create(const std::string& type_name, const std::string& s, syntax_collection& collection)
            {
                uint32_t i = 0;
                if (!collection.define(type_name, i))
                {
                    return false;
                }
                std::shared_ptr<syntax> o;
                return create(i, s, collection, o);
            }
        }
    }
}
