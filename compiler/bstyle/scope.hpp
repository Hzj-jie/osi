#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <queue>
#include <functional>
#include "../scope/scope.hpp"
#include "bstyle_forward.hpp"

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            class scope : public compiler_scope<scope, logic_writer>
            {
            public:
                static bool code_build(const std::string& source, logic_writer& o)
                {
                    return build_code(source, o);
                }

                static osi::compiler::code_gens<logic_writer>& code_gens()
                {
                    return get_bstyle_code_gens();
                }

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

                class bstyle_accessor : public accessor_t
                {
                private:
                    scope* s_{nullptr};
                public:
                    explicit bstyle_accessor(scope* s) : s_(s) { assert(s_ != nullptr); }

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
                    function_t* functions() const override { return &s_->get_root()->f_; }
                    params_t* params() const override { return &s_->ps_; }
                };

                bstyle_accessor accessor_{this};

                class bstyle_features : public features_t
                {
                public:
                    bool with_namespace() const override { return false; }
                };

                bstyle_features features_{};

            public:
                accessor_t* get_accessor() override { return &accessor_; }
                const accessor_t* get_accessor() const override { return &accessor_; }
                const features_t* get_features() const override { return &features_; }

                explicit scope(scope* parent = nullptr)
                    : compiler_scope<scope, logic_writer>(parent)
                {
                    if (is_root())
                    {
                        defines().define("BSTYLE");
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
