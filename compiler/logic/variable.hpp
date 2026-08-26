#pragma once
#include <string>
#include <vector>
#include <memory>
#include "scope.hpp"
#include "errors.hpp"
#include "../../primitive/data_ref.hpp"
#include "../../primitive/data_block.hpp"
#include "../../app_info/assert.hpp"
#include "../../utils/strutils.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class variable
            {
            public:
                std::string name;
                std::shared_ptr<variable> index;
                std::string type;
                uint32_t size{0};

                variable() = default;

                variable(std::string name, std::shared_ptr<variable> index, std::string type, uint32_t size)
                    : name(std::move(name)), index(std::move(index)), type(std::move(type)), size(size)
                {
                    assert(!this->name.empty());
                    assert(!this->type.empty());
                }

                static std::string name_of(const std::string& array, const std::string& index)
                {
                    assert(!array.empty());
                    assert(!index.empty());
                    return array + "[" + index + "]";
                }

                static bool is_heap_name(const std::string& name)
                {
                    assert(!name.empty());
                    size_t idx = name.find('[');
                    if (idx != std::string::npos)
                    {
                        assert(!name.empty() && name.back() == ']');
                        return true;
                    }
                    assert(name.find(']') == std::string::npos);
                    return false;
                }

                static bool of(const std::string& name, std::vector<std::string>* v, variable& out)
                {
                    assert(!name.empty());
                    size_t index_start = name.find('[');
                    if (index_start == std::string::npos)
                    {
                        if (name.find(']') != std::string::npos)
                        {
                            errors::invalid_variable_name(name, "Unexpected closing bracket.");
                            return false;
                        }
                        scope::exported_ref r;
                        if (!scope::current()->variables().export_var(name, r))
                        {
                            errors::variable_undefined(name);
                            return false;
                        }
                        uint32_t sz = 0;
                        if (!scope::current()->types().retrieve(r.type, sz))
                        {
                            errors::type_undefined(r.type);
                            return false;
                        }
                        out = variable(name, nullptr, r.type, sz);
                        return true;
                    }
                    else
                    {
                        assert(v != nullptr);
                        if (name.back() != ']')
                        {
                            errors::invalid_variable_name(name, "Closing bracket is not at the end of the name.");
                            return false;
                        }
                        size_t first_close = name.find(']');
                        if (first_close < index_start)
                        {
                            errors::invalid_variable_name(name, "Closing bracket is before opening bracket.");
                            return false;
                        }
                        if (index_start == name.size() - 2)
                        {
                            errors::invalid_variable_name(name, "Empty index string.");
                            return false;
                        }

                        std::string idx_str = name.substr(index_start + 1, name.size() - index_start - 2);
                        variable idx_var;
                        if (!of(idx_str, v, idx_var))
                        {
                            errors::invalid_variable_name(name, "Index cannot be parsed.");
                            return false;
                        }

                        std::string ptr_name = scope::current()->variables().unique_name();
                        if (!scope::current()->variables().define(ptr_name, scope::type_t::ptr_type))
                        {
                            return false;
                        }
                        v->push_back("push");
                        scope::current()->when_end_scope([v]()
                        {
                            v->push_back("pop");
                        });

                        scope::exported_ref d;
                        if (!scope::current()->variables().export_var(ptr_name, d))
                        {
                            return false;
                        }

                        std::string arr_name = name.substr(0, index_start);
                        scope::exported_ref r;
                        if (!scope::current()->variables().export_var(arr_name, r))
                        {
                            errors::variable_undefined(arr_name);
                            return false;
                        }

                        v->push_back(strcat("add ", d.data_ref.as_string(), " ", r.data_ref.as_string(), " ", idx_var.ToString()));

                        uint32_t sz = 0;
                        if (!scope::current()->types().retrieve(r.type, sz))
                        {
                            errors::type_undefined(r.type);
                            return false;
                        }
                        out = variable(ptr_name, std::make_shared<variable>(idx_var), r.type, sz);
                        return true;
                    }
                }

                static bool of(const std::string& name, std::vector<std::string>& v, variable& out)
                {
                    return of(name, &v, out);
                }

                bool is_zero_size() const
                {
                    return scope::type_t::is_zero_size(size);
                }

                bool is_assignable_from_size(uint32_t exp_size) const
                {
                    if (is_zero_size()) return false;
                    if (scope::type_t::is_size_or_variable(size, exp_size)) return true;
                    if (size >= exp_size) return true;
                    return false;
                }

                bool is_assignable_from(const variable& source) const
                {
                    if (is_assignable_from_size(source.size)) return true;
                    errors::raise("Source ", source.name, " [", source.type, "] and target ", name, " [", type, "] have different sizes, unassignable.");
                    return false;
                }

                bool is_assignable_from(uint32_t exp_size) const
                {
                    if (is_assignable_from_size(exp_size)) return true;
                    errors::raise("Target ", name, " [", type, "] is not assignable from byte array with size ", exp_size);
                    return false;
                }

                bool is_assignable_from_uint32() const
                {
                    if (is_assignable_from_size(sizeof(uint32_t))) return true;
                    errors::raise("Variable ", name, " [", type, "] is not assignable from uint32.");
                    return false;
                }

                bool is_assignable_to_uint32() const
                {
                    if (is_zero_size()) return false;
                    if (size <= sizeof(uint32_t) || scope::type_t::is_variable_size(size)) return true;
                    errors::raise("Variable ", name, " [", type, "] is not assignable to uint32.");
                    return false;
                }

                bool is_assignable_from_bool() const
                {
                    if (is_assignable_from_size(sizeof(uint8_t))) return true;
                    errors::raise("Variable ", name, " [", type, "] is not assignable from bool.");
                    return false;
                }

                bool is_variable_size() const
                {
                    if (is_zero_size()) return false;
                    if (scope::type_t::is_variable_size(size)) return true;
                    errors::raise("Target ", name, " [", type, "] is not a variable size type.");
                    return false;
                }

                std::string ToString() const
                {
                    scope::exported_ref d;
                    assert(scope::current()->variables().export_var(name, d));
                    if (index)
                    {
                        return d.data_ref.to_heap().as_string();
                    }
                    return d.data_ref.as_string();
                }
            };
        }
    }
}
