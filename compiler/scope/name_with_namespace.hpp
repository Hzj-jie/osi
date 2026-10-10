#pragma once
#include <string>
#include <utility>
#include "namespace.hpp"

namespace osi
{
    namespace compiler
    {
        struct name_with_namespace
        {
            std::string ns;
            std::string name;

            static name_with_namespace of(const std::string& i)
            {
                auto p = current_namespace_t::of_namespace_and_name(i);
                return {p.first, p.second};
            }

            static name_with_namespace of_global_namespace(const std::string& i)
            {
                assert(!i.empty());
                return {"", i};
            }

            std::string fully_qualified_name() const
            {
                return namespace_t::fully_qualified_name(ns, name);
            }

            bool operator==(const name_with_namespace& o) const
            {
                return ns == o.ns && name == o.name;
            }

            bool operator<(const name_with_namespace& o) const
            {
                if (ns != o.ns) return ns < o.ns;
                return name < o.name;
            }
        };
    }
}

namespace std
{
    template <>
    struct hash<osi::compiler::name_with_namespace>
    {
        size_t operator()(const osi::compiler::name_with_namespace& n) const
        {
            return std::hash<std::string>()(n.ns) ^ (std::hash<std::string>()(n.name) << 1);
        }
    };
}
