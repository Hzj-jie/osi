#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../read_scoped.hpp"
#include "../logic/builders.hpp"
#include "../logic/logic_writer.hpp"
#include "struct_def.hpp"
#include "compiler_scope_interface.hpp"
#include "accessor.hpp"
#include "normalized.hpp"
#include "struct.hpp"
#include "variable.hpp"
#include "temp_logic_name.hpp"

namespace osi
{
    namespace compiler
    {
        class value_target_t
        {
        public:
            struct target
            {
                std::string type;
                std::vector<std::string> names;

                const std::string& only() const
                {
                    assert(names.size() == 1);
                    return names[0];
                }
            };

        private:
            read_scoped<target> values_;
            read_scoped<std::vector<std::string>> value_lists_;

        public:
            std::vector<std::string> with_value(const std::string& type, std::vector<std::string> vs)
            {
                std::string t = normalized_type::parameter_type_of(type).full_type();
                values_.push(target{t, vs});
                return vs;
            }

            std::vector<std::string> with_value(const std::string& type, const std::vector<parameter>& ps)
            {
                std::vector<std::string> names;
                names.reserve(ps.size());
                for (const auto& p : ps)
                {
                    names.push_back(p.name);
                }
                return with_value(type, std::move(names));
            }

            read_scoped<target>::ref value()
            {
                return values_.pop();
            }

            class primitive_ref
            {
            private:
                read_scoped<target>::ref ref_;

            public:
                primitive_ref() = default;
                explicit primitive_ref(read_scoped<target>::ref r) : ref_(std::move(r)) {}

                bool retrieve(std::string& o) const
                {
                    if (ref_->names.size() != 1)
                    {
                        return false;
                    }
                    o = ref_->names[0];
                    return true;
                }
            };

            primitive_ref primitive_type()
            {
                return primitive_ref(values_.pop());
            }

            void with_value_list(std::vector<std::string> v)
            {
                value_lists_.push(std::move(v));
            }

            read_scoped<std::vector<std::string>>::ref value_list()
            {
                return value_lists_.pop();
            }

            std::vector<std::string> with_temp_target(const std::string& type, logic_writer& o)
            {
                auto* cur = compiler_scope_interface::current();
                auto define_primitive_type_temp_target = [&](const std::string& t_raw, const std::string& n) {
                    std::string t = normalized_type::parameter_type_of(t_raw).full_type();
                    assert(!cur->myself().structs()->defined(t));
                    assert(!cur->myself().variables()->defined(n));
                    assert(cur->myself().variables()->define(t, n));
                    assert(builders::of_define(n, t).to(o));
                };

                struct_def params;
                std::string temp_var_name = cur->myself().temp_logic_name()->variable();
                if (cur->myself().structs()->resolve(type, temp_var_name, params))
                {
                    for (const auto& p : params.primitives())
                    {
                        define_primitive_type_temp_target(p.non_ref_type(), p.name);
                    }
                    return with_value(type, params.primitives());
                }

                define_primitive_type_temp_target(type, temp_var_name);
                with_value(type, std::vector<std::string>{temp_var_name});
                return std::vector<std::string>{temp_var_name};
            }
        };
    }
}
