#pragma once
#include <string>
#include "../../app_info/assert.hpp"

namespace osi
{
    namespace automata
    {
        class typed_node;
    }

    namespace compiler
    {
        class accessor_t;
        class features_t;
        class struct_def;

        class compiler_scope_interface
        {
        public:
            virtual ~compiler_scope_interface() = default;

            virtual accessor_t& myself() = 0;
            virtual const accessor_t& myself() const = 0;
            virtual const features_t& features() const = 0;
            virtual bool is_root() const = 0;
            virtual std::string type_alias(std::string alias) const = 0;
            virtual bool structs_resolve(const std::string& type, const std::string& name, struct_def& o) const = 0;
            virtual bool structs_is_type_defined(const std::string& type) const = 0;
            virtual std::string resolve_type_node(const std::shared_ptr<automata::typed_node>& type) const { return ""; }
            virtual std::string current_function_name() const { return ""; }

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
