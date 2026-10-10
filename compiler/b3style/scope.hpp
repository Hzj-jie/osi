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
#include "../read_scoped.hpp"
#include "../logic/logic_writer.hpp"
#include "../logic/builders.hpp"
#include "../logic/scope.hpp"
#include "../code_gen/code_gen.hpp"
#include "../code_gen/statements.hpp"
#include "../../automata/typed_node.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../../utils/strutils.hpp"
#include "b3style_forward.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class scope;

            inline thread_local bool _disable_namespace = false;

            class disable_namespace_guard
            {
            private:
                bool prev_{false};
            public:
                disable_namespace_guard()
                {
                    assert(!_disable_namespace);
                    prev_ = _disable_namespace;
                    _disable_namespace = true;
                }
                ~disable_namespace_guard()
                {
                    _disable_namespace = prev_;
                }
            };

            inline disable_namespace_guard disable_namespace()
            {
                return disable_namespace_guard();
            }

            inline bool with_namespace()
            {
                return !_disable_namespace;
            }

            class scope : public compiler_scope<scope>
            {
            private:
                includes_t incs_;
                call_hierarchy_t fc_;
                define_t d_;
                type_alias_t ta_;
                struct_t s_;
                variable_t v_;
                function_t f_;
                value_target_t vt_;
                params_t ps_;
                current_function_t cf_;
                delegate_t de_;
                temp_logic_name_t t_;
                current_namespace_t cn_;
                template_t tt_;
                class_t c_;
                root_type_injector_t i_;

                class b3style_accessor : public accessor_t
                {
                private:
                    scope* s_{nullptr};
                public:
                    explicit b3style_accessor(scope* s) : s_(s) { assert(s_ != nullptr); }

                    includes_t* includes() const override { return &s_->get_root()->incs_; }
                    define_t* defines() const override { return &s_->get_root()->d_; }
                    type_alias_t* type_alias() const override { return &s_->ta_; }
                    current_function_t* current_function() const override { return &s_->cf_; }
                    void current_function(const current_function_t& c) override { s_->cf_ = c; }
                    std::string current_function_name() const override { return s_->cf_.name(); }
                    delegate_t* delegates() const override { return &s_->de_; }
                    struct_t* structs() const override { return &s_->s_; }
                    variable_t* variables() const override { return &s_->v_; }
                    temp_logic_name_t* temp_logic_name() const override { return &s_->get_root()->t_; }
                    value_target_t* value_target() const override { return &s_->vt_; }
                    call_hierarchy_t* call_hierarchy() const override { return &s_->get_root()->fc_; }
                    current_namespace_t* current_namespace() const override { return &s_->get_root()->cn_; }
                    root_type_injector_t* root_type_injector() const override { return &s_->get_root()->i_; }
                    class_t* classes() const override { return &s_->get_root()->c_; }
                    template_t* template_table() const override { return &s_->get_root()->tt_; }
                    function_t* functions() const override { return &s_->get_root()->f_; }
                    params_t* params() const override { return &s_->ps_; }
                };

                b3style_accessor accessor_{this};

                class b3style_features : public features_t
                {
                public:
                    bool with_type_alias() const override { return true; }
                    bool with_namespace() const override { return !_disable_namespace; }
                };

                b3style_features features_{};

            public:
                accessor_t* get_accessor() override { return &accessor_; }
                const accessor_t* get_accessor() const override { return &accessor_; }
                const features_t* get_features() const override { return &features_; }

                std::string resolve_type_node_impl(const std::shared_ptr<automata::typed_node>& type) const
                {
                    assert(type != nullptr);
                    if (type->type_name == "template-type-name" ||
                        (!type->leaf() && type->child_count() > 0 && type->child(0)->type_name == "template-type-name"))
                    {
                        auto target_node = (type->type_name == "template-type-name") ? type : type->child(0);
                        std::string s;
                        if (code_gen_of(target_node).dump(s))
                        {
                            while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\n' || s.back() == '\r')) s.pop_back();
                            while (!s.empty() && (s.front() == ' ' || s.front() == '\t' || s.front() == '\n' || s.front() == '\r')) s.erase(s.begin());
                            if (!s.empty())
                            {
                                return s;
                            }
                        }
                    }
                    return "";
                }

                std::string resolve_type_node(const std::shared_ptr<automata::typed_node>& type) const override
                {
                    return resolve_type_node_impl(type);
                }

                explicit scope(scope* parent = nullptr)
                    : compiler_scope<scope>(parent)
                {
                    if (is_root())
                    {
                        defines().define("B3STYLE");
                        tt_.set_code_builder([](const std::string& s) {
                            return build_code(s, *scope::current()->root_type_injector().current());
                        });
                    }
                }

                ~scope() override
                {
                    this->run_end_scope();
                }
            };
        }
    }
}
