#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../function_signature.hpp"
#include "../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        class struct_def
        {
        private:
            std::vector<parameter> nesteds_;
            std::vector<parameter> primitives_;

        public:
            struct_def() = default;

            struct_def(std::vector<parameter> nesteds, std::vector<parameter> primitives)
                : nesteds_(std::move(nesteds)), primitives_(std::move(primitives))
            {
                for (const auto& p : nesteds_)
                {
                    assert(!p.ref);
                }
                for (const auto& p : primitives_)
                {
                    assert(!p.ref);
                }
            }

            const std::vector<parameter>& nesteds() const
            {
                return nesteds_;
            }

            const std::vector<parameter>& primitives() const
            {
                return primitives_;
            }

            uint32_t primitive_count() const
            {
                return static_cast<uint32_t>(primitives_.size());
            }

            static struct_def of_primitive(const std::string& type, const std::string& name)
            {
                struct_def d;
                d.with_primitive(type, name);
                return d;
            }

            struct_def& with_nested(const std::string& type, const std::string& name)
            {
                nesteds_.push_back(nested(type, name));
                return *this;
            }

            static parameter nested(const std::string& type, const std::string& name)
            {
                return parameter::non_ref(type, name);
            }

            static parameter nested(const parameter& p)
            {
                assert(!p.ref);
                return nested(p.non_ref_type(), p.name);
            }

            struct_def& with_primitive(const std::string& type, const std::string& name)
            {
                primitives_.push_back(nested(type, name));
                return *this;
            }

            struct_def append_prefix(const std::string& prefix) const
            {
                std::vector<parameter> n;
                n.reserve(nesteds_.size());
                for (const auto& p : nesteds_)
                {
                    n.push_back(p.map_name([&prefix](const std::string& name) {
                        return prefix + "." + name;
                    }));
                }

                std::vector<parameter> pr;
                pr.reserve(primitives_.size());
                for (const auto& p : primitives_)
                {
                    pr.push_back(p.map_name([&prefix](const std::string& name) {
                        return prefix + "." + name;
                    }));
                }

                return struct_def(std::move(n), std::move(pr));
            }

            void append(const struct_def& r)
            {
                nesteds_.insert(nesteds_.end(), r.nesteds_.begin(), r.nesteds_.end());
                primitives_.insert(primitives_.end(), r.primitives_.begin(), r.primitives_.end());
            }

            template <typename Func>
            bool for_each_primitive(Func&& f) const
            {
                for (const auto& p : primitives_)
                {
                    if (!f(p)) return false;
                }
                return true;
            }
        };
    }
}
