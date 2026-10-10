#pragma once
#include <string>
#include <vector>
#include "compiler_scope_interface.hpp"
#include "accessor.hpp"
#include "struct.hpp"
#include "../function_signature.hpp"
#include "../logic/builders.hpp"
#include "../logic/scope.hpp"

namespace osi
{
    namespace compiler
    {
        class current_function_t
        {
        private:
            std::string name_;
            std::string return_type_;
            std::vector<parameter> parameters_;

        public:
            current_function_t() = default;

            current_function_t(std::string n, std::string rt, std::vector<parameter> ps)
                : name_(std::move(n)), return_type_(std::move(rt)), parameters_(std::move(ps))
            {
                assert(!name_.empty());
                assert(!return_type_.empty());
            }

            const std::string& name() const { return name_; }
            const std::string& return_type() const { return return_type_; }
            const std::vector<parameter>& parameters() const { return parameters_; }
            const std::vector<parameter>& params() const { return parameters_; }

            bool has() const { return !name_.empty(); }

            bool allow_return_value() const
            {
                return return_type_ != "void" && return_type_ != "::void" &&
                       return_type_ != "type0" && return_type_ != "::type0";
            }

            bool return_struct() const
            {
                auto* cur = compiler_scope_interface::current_or_null();
                if (!cur) return false;
                auto* st = cur->myself().structs();
                if (!st) return false;
                return st->defined(return_type());
            }

            std::string signature() const
            {
                return function_signature<parameter>(name_, return_type_, parameters_).ToString();
            }
        };
    }
}
