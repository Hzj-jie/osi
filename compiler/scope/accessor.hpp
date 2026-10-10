#pragma once
#include <string>
#include <memory>
#include "../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        class includes_t;
        class define_t;
        class type_alias_t;
        class struct_t;
        class variable_t;
        class function_t;
        class value_target_t;
        class params_t;
        class current_function_t;
        class delegate_t;
        class temp_logic_name_t;
        class call_hierarchy_t;
        class current_namespace_t;
        class class_t;
        class template_t;
        template <typename Writer>
        class root_type_injector_t;

        class accessor_t
        {
        public:
            virtual ~accessor_t() = default;

            virtual includes_t* includes() const { assert(false); return nullptr; }
            virtual define_t* defines() const { assert(false); return nullptr; }
            virtual type_alias_t* type_alias() const { assert(false); return nullptr; }
            virtual current_function_t* current_function() const { assert(false); return nullptr; }
            virtual void current_function(const current_function_t&) { assert(false); }
            virtual std::string current_function_name() const { assert(false); return ""; }
            virtual delegate_t* delegates() const { assert(false); return nullptr; }
            virtual struct_t* structs() const { assert(false); return nullptr; }
            virtual variable_t* variables() const { assert(false); return nullptr; }
            virtual temp_logic_name_t* temp_logic_name() const { assert(false); return nullptr; }
            virtual value_target_t* value_target() const { assert(false); return nullptr; }
            virtual call_hierarchy_t* call_hierarchy() const { assert(false); return nullptr; }
            virtual current_namespace_t* current_namespace() const { assert(false); return nullptr; }
            virtual class_t* classes() const { assert(false); return nullptr; }
            virtual template_t* template_table() const { assert(false); return nullptr; }
            virtual function_t* functions() const { assert(false); return nullptr; }
            virtual params_t* params() const { assert(false); return nullptr; }
        };

        template <typename Writer>
        class writer_accessor_t : public accessor_t
        {
        public:
            virtual root_type_injector_t<Writer>* root_type_injector() const { assert(false); return nullptr; }
        };
    }
}
