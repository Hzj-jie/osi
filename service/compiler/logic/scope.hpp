#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../scope/scope.hpp"
#include "../function_signature.hpp"
#include "errors.hpp"
#include "../../../primitive/data_ref.hpp"
#include "../../../primitive/interrupts.hpp"
#include "../../../app_info/assert.hpp"
#include "../../../utils/strutils.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class scope : public scope_base<scope>
            {
            public:
                class typed_ref
                {
                public:
                    std::string type;

                    typed_ref() = default;
                    explicit typed_ref(std::string t) : type(std::move(t))
                    {
                        assert(!type.empty());
                    }
                };

                class ref : public typed_ref
                {
                public:
                    uint64_t offset{0}; // 1-based offset within scope

                    ref() = default;
                    ref(uint64_t off, std::string t) : typed_ref(std::move(t)), offset(off) {}

                    ref with(std::string t) const
                    {
                        return ref(offset, std::move(t));
                    }
                };

                class exported_ref : public typed_ref
                {
                public:
                    primitive::data_ref data_ref;

                    exported_ref() = default;
                    exported_ref(const ref& r, primitive::data_ref dr)
                        : typed_ref(r.type), data_ref(dr) {}
                };

                class type_t
                {
                public:
                    inline static const std::string variable_type = "type*";
                    inline static const std::string ptr_type = "type_ptr";
                    inline static const std::string zero_type = "type0";
                    constexpr static uint32_t variable_size = 0x7FFFFFFF;
                    constexpr static uint32_t ptr_size = sizeof(uint64_t);
                    constexpr static uint32_t zero_size = 0;

                private:
                    std::unordered_map<std::string, uint32_t> sizes;

                public:
                    type_t()
                    {
                        assert_define(variable_type, variable_size);
                        assert_define(ptr_type, ptr_size);
                        assert_define(zero_type, zero_size);
                    }

                    bool define(const std::string& type, uint32_t size)
                    {
                        assert(!type.empty());
                        if (size == variable_size && type != variable_type) return false;
                        if (size == zero_size && type != zero_type) return false;
                        if (sizes.find(type) != sizes.end()) return false;
                        sizes[type] = size;
                        sizes["::" + type] = size;
                        return true;
                    }

                    void assert_define(const std::string& type, uint32_t size)
                    {
                        assert(define(type, size));
                    }

                    static bool is_variable_size(uint32_t size)
                    {
                        return size >= variable_size;
                    }

                    static bool is_zero_size(uint32_t size)
                    {
                        return size == zero_size;
                    }

                    static bool is_size_or_variable(uint32_t size, uint32_t exp_size)
                    {
                        return size == exp_size || is_variable_size(size);
                    }

                    bool retrieve(const std::string& type, uint32_t& size) const
                    {
                        auto it = sizes.find(type);
                        if (it != sizes.end())
                        {
                            size = it->second;
                            return true;
                        }
                        return false;
                    }

                    uint32_t operator[](const std::string& type) const
                    {
                        uint32_t s = 0;
                        assert(retrieve(type, s));
                        return s;
                    }
                };

                class anchor : public function_signature<parameter>
                {
                public:
                    uint32_t begin{0};

                    anchor() = default;
                    anchor(std::string name, uint32_t begin, std::string return_type, std::vector<parameter> parameters)
                        : function_signature<parameter>(std::move(name), std::move(return_type), std::move(parameters)),
                          begin(begin) {}

                    primitive::data_ref begin_ref() const
                    {
                        return primitive::data_ref::abs(begin);
                    }
                };

                class anchor_t
                {
                private:
                    std::unordered_map<std::string, anchor> m;

                public:
                    bool define(const std::string& name, uint32_t pos, const std::string& return_type, const std::vector<parameter>& parameters)
                    {
                        assert(!name.empty());
                        assert(!return_type.empty());
                        anchor a(name, pos, return_type, parameters);
                        auto it = m.find(name);
                        if (it != m.end())
                        {
                            errors::raise("Anchor ", name, " redefined at abs", pos, ", last position is abs", it->second.begin);
                            return false;
                        }
                        m[name] = std::move(a);
                        return true;
                    }

                    bool of(const std::string& name, anchor& out) const
                    {
                        auto it = m.find(name);
                        if (it != m.end())
                        {
                            out = it->second;
                            return true;
                        }
                        return false;
                    }
                };

                class anchor_ref : public function_signature<parameter_type>
                {
                public:
                    anchor_ref() = default;
                    anchor_ref(std::string name, std::string return_type, std::vector<parameter_type> parameters)
                        : function_signature<parameter_type>(std::move(name), std::move(return_type), std::move(parameters)) {}
                };

                class anchor_ref_t
                {
                private:
                    std::unordered_map<std::string, anchor_ref> decls;
                    std::unordered_map<std::string, anchor_ref> defs;

                public:
                    bool decl(const std::string& type, const std::string& return_type, const std::vector<parameter_type>& parameters)
                    {
                        assert(!type.empty());
                        assert(!return_type.empty());
                        if (decls.find(type) != decls.end())
                        {
                            errors::raise("AnchorRef ", type, " has been defined already.");
                            return false;
                        }
                        decls[type] = anchor_ref(type, return_type, parameters);
                        return true;
                    }

                    void define(const std::string& name, anchor_ref ar)
                    {
                        defs[name] = std::move(ar);
                    }

                    bool find_decl(const std::string& type, anchor_ref& out) const
                    {
                        auto it = decls.find(type);
                        if (it != decls.end())
                        {
                            out = it->second;
                            return true;
                        }
                        return false;
                    }

                    bool of(const std::string& name, anchor_ref& out) const
                    {
                        auto it = defs.find(name);
                        if (it != defs.end())
                        {
                            out = it->second;
                            return true;
                        }
                        return false;
                    }
                };

            private:
                class variable_t
                {
                private:
                    std::unordered_map<std::string, ref> stack;
                    uint32_t undefined{0};

                public:
                    bool find(const std::string& name, ref& out) const
                    {
                        auto it = stack.find(name);
                        if (it != stack.end())
                        {
                            out = it->second;
                            return true;
                        }
                        return false;
                    }

                    bool undefine(const std::string& name)
                    {
                        if (stack.erase(name) > 0)
                        {
                            undefined++;
                            return true;
                        }
                        return false;
                    }

                    uint32_t size() const
                    {
                        return static_cast<uint32_t>(stack.size()) + undefined;
                    }

                    bool define(const std::string& name, const std::string& type)
                    {
                        assert(!name.empty());
                        assert(!type.empty());
                        auto it = stack.find(name);
                        if (it != stack.end())
                        {
                            errors::redefine(name, type, it->second.type);
                            return false;
                        }
                        stack[name] = ref(size() + 1, type);
                        return true;
                    }

                    bool redefine(const std::string& name, const std::string& type)
                    {
                        auto it = stack.find(name);
                        if (it == stack.end()) return false;
                        it->second = it->second.with(type);
                        return true;
                    }
                };

                variable_t v;
                std::shared_ptr<type_t> t;
                std::shared_ptr<anchor_t> a;
                anchor_ref_t ar;
                std::shared_ptr<primitive::interrupts> f;

            public:
                struct variable_proxy
                {
                private:
                    scope* s;

                public:
                    explicit variable_proxy(scope* s) : s(s) { assert(s != nullptr); }

                    bool define(const std::string& name, const std::string& type)
                    {
                        return s->v.define(name, type);
                    }

                    bool defined_in_current_scope(const std::string& name) const
                    {
                        ref dummy;
                        return s->v.find(name, dummy);
                    }

                    bool undefine(const std::string& name)
                    {
                        scope* cur = s;
                        while (cur != nullptr)
                        {
                            if (cur->v.undefine(name)) return true;
                            cur = cur->parent;
                        }
                        errors::raise("Cannot find variable ", name);
                        return false;
                    }

                    bool redefine(const std::string& name, const std::string& type)
                    {
                        scope* cur = s;
                        while (cur != nullptr)
                        {
                            if (cur->v.redefine(name, type)) return true;
                            cur = cur->parent;
                        }
                        errors::raise("Cannot find variable ", name);
                        return false;
                    }

                    bool export_var(const std::string& name, exported_ref& out) const
                    {
                        uint64_t child_scopes_size = 0;
                        const scope* cur = s;
                        while (cur != nullptr)
                        {
                            ref r;
                            if (!cur->v.find(name, r))
                            {
                                child_scopes_size += cur->v.size();
                                cur = cur->parent;
                                continue;
                            }
                            primitive::data_ref d;
                            if (cur->is_root())
                            {
                                if (!primitive::data_ref::abs(r.offset - 1, d)) return false;
                            }
                            else
                            {
                                if (!primitive::data_ref::rel(cur->v.size() - r.offset + child_scopes_size, d)) return false;
                            }
                            out = exported_ref(r, d);
                            return true;
                        }
                        return false;
                    }

                    exported_ref export_var(const std::string& name) const
                    {
                        exported_ref out;
                        assert(export_var(name, out));
                        return out;
                    }

                    std::string unique_name() const
                    {
                        return "@scope_" + std::to_string(reinterpret_cast<uintptr_t>(&s->v)) +
                               "_unique_name_" + std::to_string(s->v.size() + 1);
                    }
                };

                struct anchor_ref_proxy
                {
                private:
                    scope* s;

                public:
                    explicit anchor_ref_proxy(scope* s) : s(s) { assert(s != nullptr); }

                    bool decl(const std::string& type, const std::string& return_type, const std::vector<parameter_type>& parameters)
                    {
                        return s->get_root()->ar.decl(type, return_type, parameters);
                    }

                    bool define(const std::string& type, const std::string& name)
                    {
                        anchor_ref ar;
                        scope* cur = s;
                        while (cur != nullptr)
                        {
                            if (cur->ar.find_decl(type, ar))
                            {
                                s->ar.define(name, std::move(ar));
                                return true;
                            }
                            cur = cur->parent;
                        }
                        return false;
                    }

                    bool of(const std::string& name, anchor_ref& out) const
                    {
                        const scope* cur = s;
                        while (cur != nullptr)
                        {
                            if (cur->ar.of(name, out)) return true;
                            cur = cur->parent;
                        }
                        return false;
                    }
                };

                explicit scope(std::shared_ptr<primitive::interrupts> functions)
                    : scope_base<scope>(nullptr),
                      t(std::make_shared<type_t>()),
                      a(std::make_shared<anchor_t>()),
                      f(std::move(functions))
                {
                    if (!f) f = std::make_shared<primitive::interrupts>();
                }

                explicit scope(scope* parent_ptr)
                    : scope_base<scope>(parent_ptr)
                {
                }

                scope()
                    : scope_base<scope>(current_or_null())
                {
                    if (parent == nullptr)
                    {
                        t = std::make_shared<type_t>();
                        a = std::make_shared<anchor_t>();
                        f = std::make_shared<primitive::interrupts>();
                    }
                }

                ~scope() override
                {
                    this->run_end_scope();
                }

                variable_proxy variables()
                {
                    return variable_proxy(this);
                }

                anchor_ref_proxy anchor_refs()
                {
                    return anchor_ref_proxy(this);
                }

                type_t& types()
                {
                    return *get_root()->t;
                }

                const type_t& types() const
                {
                    return *get_root()->t;
                }

                anchor_t& anchors()
                {
                    return *get_root()->a;
                }

                const anchor_t& anchors() const
                {
                    return *get_root()->a;
                }

                primitive::interrupts& functions()
                {
                    return *get_root()->f;
                }

                const primitive::interrupts& functions() const
                {
                    return *get_root()->f;
                }
            };
        }
    }
}
