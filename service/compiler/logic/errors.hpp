#pragma once
#include <string>
#include <vector>
#include "../../../app_info/error_handle.hpp"
#include "../../../utils/strutils.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class errors
            {
            public:
                template <typename... Args>
                static void raise(Args&&... args)
                {
                    raise_error(strcat("[Logic] ", std::forward<Args>(args)...));
                }

                static void variable_undefined(const std::string& variable)
                {
                    raise("Variable ", variable, " is undefined.");
                }

                static void interrupt_undefined(const std::string& name)
                {
                    raise("Interrupt function ", name, " is undefined.");
                }

                static void redefine(const std::string& name, const std::string& type, const std::string& last_type)
                {
                    raise("Variable ", name, " [", type, "] redefined, last type ", last_type);
                }

                static void type_undefined(const std::string& type, const std::string& target)
                {
                    raise("Type ", type, " referred by target ", target, " is undefined.");
                }

                static void type_undefined(const std::string& type)
                {
                    raise("Type ", type, " is undefined.");
                }

                static void anchor_undefined(const std::string& name)
                {
                    raise("Anchor ", name, " is not defined.");
                }

                static void anchor_ref_undefined(const std::string& name)
                {
                    raise("AnchorRef ", name, " is not defined.");
                }

                static void unexpected_token(const std::string& s)
                {
                    raise("Unexpected token ", s);
                }

                static void typed_parameters_is_not_closed()
                {
                    raise("Right parenthesis is not found, typed_parameters is not closed.");
                }

                static void parameters_is_not_closed()
                {
                    raise("Right parenthesis is not found, parameters is not closed.");
                }

                static void paragraph_is_not_closed()
                {
                    raise("Right bracket is not found, paragraph is not closed.");
                }

                static void invalid_variable_name(const std::string& name, const std::string& msg)
                {
                    raise("Variable name ", name, " is invalid. ", msg);
                }

                static void not_a_stack_var(const std::string& name)
                {
                    raise("Variable ", name, " is not a variable on stack, e.g. it's in format of var[index].");
                }
            };
        }
    }
}
