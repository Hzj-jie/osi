#pragma once
#include <string>
#include "../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        class accessor_t;
        class features_t;

        class compiler_scope_interface
        {
        public:
            virtual ~compiler_scope_interface() = default;

            virtual accessor_t& myself() = 0;
            virtual const accessor_t& myself() const = 0;
            virtual const features_t& features() const = 0;
            virtual bool is_root() const = 0;
            virtual std::string type_alias(std::string alias) const = 0;

            static compiler_scope_interface* current_or_null();
            static compiler_scope_interface* current();
            static void set_current(compiler_scope_interface* s);
        };

        inline thread_local compiler_scope_interface* in_thread_compiler_scope{nullptr};

        inline compiler_scope_interface* compiler_scope_interface::current_or_null()
        {
            return in_thread_compiler_scope;
        }

        inline compiler_scope_interface* compiler_scope_interface::current()
        {
            assert(in_thread_compiler_scope != nullptr);
            return in_thread_compiler_scope;
        }

        inline void compiler_scope_interface::set_current(compiler_scope_interface* s)
        {
            in_thread_compiler_scope = s;
        }
    }
}
