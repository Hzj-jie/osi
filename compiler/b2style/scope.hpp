#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <queue>
#include <functional>
#include <algorithm>
#include <sstream>
#include "../scope/scope.hpp"
#include "../scope/struct_def.hpp"
#include "../function_signature.hpp"
#include "../code_gen/statements.hpp"
#include "b2style_forward.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../../utils/strutils.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {
            class scope;

            // Forward declaration of build_code for recursive code builds (templates, classes, etc.)
            bool build_code(const std::string& input, rewriter::typed_node_writer& o);

            using current_file_guard = bstyle_compiler::current_file_guard;

            namespace _namespace
            {
                class bstyle_format
                {
                private:
                    inline static const std::string namespace_replacer = "__";

                public:
                    static std::string of(const std::string& i)
                    {
                        if (i.empty()) return i;
                        std::string base = current_namespace_t::of(i);
                        std::vector<std::string> parts;
                        std::stringstream ss(base);
                        std::string item;
                        while (std::getline(ss, item, '.'))
                        {
                            parts.push_back(item);
                        }
                        if (parts.empty()) parts.push_back(base);

                        std::string res;
                        for (size_t p = 0; p < parts.size(); ++p)
                        {
                            if (p > 0) res += '.';
                            const std::string& x = parts[p];
                            if (x.find(current_namespace_t::namespace_separator) == std::string::npos)
                            {
                                res += x;
                            }
                            else
                            {
                                std::string q = current_namespace_t::of(x);
                                size_t pos = 0;
                                while ((pos = q.find(current_namespace_t::namespace_separator, pos)) != std::string::npos)
                                {
                                    q.replace(pos, current_namespace_t::namespace_separator.length(), namespace_replacer);
                                    pos += namespace_replacer.length();
                                }
                                if (q.rfind(namespace_replacer, 0) == 0)
                                {
                                    q = q.substr(namespace_replacer.length());
                                }
                                res += q;
                            }
                        }
                        return res;
                    }

                    static std::string of(const std::shared_ptr<automata::typed_node>& n)
                    {
                        assert(n != nullptr);
                        return of(n->input_without_ignored());
                    }

                    static std::string fully_qualified_name(const std::string& i)
                    {
                        return of(namespace_t::fully_qualified_name(i));
                    }

                    static std::string operator_function_name(const std::string& operator_name)
                    {
                        assert(!operator_name.empty());
                        std::string op = operator_name;
                        for (char& c : op)
                        {
                            if (c == '-') c = '_';
                        }
                        return of(namespace_t::fully_qualified_name("b2style", op));
                    }
                };
            }

            class call_hierarchy_t : public osi::compiler::call_hierarchy_t
            {
            public:
                using calculator = calculator_t<rewriter::typed_node_writer>;

                void to_bstyle_function(const std::string& name)
                {
                    assert(!name.empty());
                    osi::compiler::call_hierarchy_t::to(name);
                }

                void to(const std::string& name)
                {
                    to_bstyle_function(_namespace::bstyle_format::of(name));
                }

                static std::function<bool(const std::shared_ptr<automata::typed_node>&)> from_value_clause();
            };

            class root_type_injector_t
            {
            private:
                std::shared_ptr<rewriter::typed_node_writer> current_;

            public:
                void _new(rewriter::typed_node_writer& o)
                {
                    current_ = std::make_shared<rewriter::typed_node_writer>();
                    o.append(current_);
                }

                std::shared_ptr<rewriter::typed_node_writer> current()
                {
                    assert(current_ != nullptr);
                    return current_;
                }
            };

            class current_function_proxy
            {
            private:
                scope* s_{nullptr};

            public:
                explicit current_function_proxy(scope* s) : s_(s) { assert(s_ != nullptr); }

                scope* define(const std::string& name);
                std::string name() const;
            };

            class variable_proxy
            {
            public:
                static std::function<bool(const std::shared_ptr<automata::typed_node>&)> define();
            };

            class scope : public compiler_scope<scope>
            {
            public:
                using call_hierarchy_t = b2style_compiler::call_hierarchy_t;
                using root_type_injector_t = b2style_compiler::root_type_injector_t;

            private:
                includes_t incs_;
                call_hierarchy_t fc_;
                current_namespace_t cn_;
                define_t d_;
                root_type_injector_t i_;
                template_t t_;
                variable_t v_;
                class_t c_;
                std::string cur_func_;

                class b2style_accessor : public accessor_t
                {
                private:
                    scope* s_{nullptr};
                public:
                    explicit b2style_accessor(scope* s) : s_(s) { assert(s_ != nullptr); }

                    includes_t* includes() const override { return &s_->get_root()->incs_; }
                    define_t* defines() const override { return &s_->get_root()->d_; }
                    std::string current_function_name() const override { return s_->cur_func_; }
                    osi::compiler::call_hierarchy_t* call_hierarchy() const override { return &s_->get_root()->fc_; }
                    current_namespace_t* current_namespace() const override { return &s_->get_root()->cn_; }
                    variable_t* variables() const override { return &s_->v_; }
                    class_t* classes() const override { return &s_->get_root()->c_; }
                    template_t* template_table() const override { return &s_->get_root()->t_; }
                };

                b2style_accessor accessor_{this};

                class b2style_features : public features_t
                {
                public:
                    bool with_type_alias() const override { return false; }
                    bool with_namespace() const override { return true; }
                };

                b2style_features features_{};

            public:
                accessor_t* get_accessor() override { return &accessor_; }
                const accessor_t* get_accessor() const override { return &accessor_; }
                const features_t* get_features() const override { return &features_; }

                explicit scope(scope* parent = nullptr)
                    : compiler_scope<scope>(parent)
                {
                    if (is_root())
                    {
                        defines().define("B2STYLE");
                        t_.set_code_builder([](const std::string& s) {
                            return build_code(s, *scope::current()->root_type_injector().current());
                        });
                    }
                }

                ~scope() override
                {
                    this->run_end_scope();
                }

                current_namespace_t& current_namespace()
                {
                    return get_root()->cn_;
                }

                b2style_compiler::root_type_injector_t& root_type_injector()
                {
                    return get_root()->i_;
                }

                template_t& template_table()
                {
                    return get_root()->t_;
                }

                class_t& classes()
                {
                    return get_root()->c_;
                }

                variable_t& variables()
                {
                    return v_;
                }

                call_hierarchy_t& call_hierarchy()
                {
                    return get_root()->fc_;
                }

                bool has_current_function() const
                {
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (!s->cur_func_.empty()) return true;
                        s = s->get_parent();
                    }
                    return false;
                }

                std::string current_function_name() const
                {
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (!s->cur_func_.empty()) return s->cur_func_;
                        s = s->get_parent();
                    }
                    return "";
                }

                std::string current_function_name_or(const std::string& def) const
                {
                    std::string f = current_function_name();
                    return f.empty() ? def : f;
                }

                current_function_proxy current_function()
                {
                    return current_function_proxy(this);
                }

                void set_current_function_name(const std::string& name)
                {
                    cur_func_ = name;
                }
            };

            inline scope* current_function_proxy::define(const std::string& name)
            {
                assert(!name.empty());
                s_->set_current_function_name(name);
                return s_;
            }

            inline std::string current_function_proxy::name() const
            {
                return s_->current_function_name();
            }

            inline std::function<bool(const std::shared_ptr<automata::typed_node>&)> call_hierarchy_t::from_value_clause()
            {
                return [](const std::shared_ptr<automata::typed_node>& n) -> bool {
                    assert(n != nullptr);
                    std::shared_ptr<automata::typed_node> o;
                    if (n->last_child()->only_descendant(o) && o->type_name == "raw-name")
                    {
                        scope::current()->call_hierarchy().to(o->word().str());
                    }
                    return true;
                };
            }

            inline std::function<bool(const std::shared_ptr<automata::typed_node>&)> variable_proxy::define()
            {
                return [](const std::shared_ptr<automata::typed_node>& n) -> bool {
                    assert(n != nullptr);
                    assert(n->child_count() >= 2);
                    return scope::current()->variables().define(
                        n->child(0)->input_without_ignored(),
                        n->child(1)->input_without_ignored());
                };
            }
        }
    }
}
