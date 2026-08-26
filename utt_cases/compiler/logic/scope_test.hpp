#pragma once
#include "../../../compiler/logic/scope.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace logic_test
{
    class scope_test : public icase
    {
    public:
        bool run() override
        {
            using namespace osi::compiler::logic;

            // Test root scope
            {
                scope s;
                utt_assert.is_true(s.is_root());
                utt_assert.is_true(s.variables().define("a", scope::type_t::variable_type));
                utt_assert.is_true(s.variables().define("b", scope::type_t::ptr_type));
                utt_assert.is_false(s.variables().define("a", scope::type_t::variable_type)); // redefine in same scope fails

                scope::exported_ref exp_a;
                utt_assert.is_true(s.variables().export_var("a", exp_a));
                utt_assert.equal(exp_a.data_ref.as_string(), std::string("abs0"));
                utt_assert.equal(exp_a.type, scope::type_t::variable_type);

                scope::exported_ref exp_b;
                utt_assert.is_true(s.variables().export_var("b", exp_b));
                utt_assert.equal(exp_b.data_ref.as_string(), std::string("abs1"));
                utt_assert.equal(exp_b.type, scope::type_t::ptr_type);

                // Nested scope
                {
                    scope child(&s);
                    utt_assert.is_false(child.is_root());
                    utt_assert.is_true(child.variables().define("c", scope::type_t::variable_type));

                    scope::exported_ref exp_c;
                    utt_assert.is_true(child.variables().export_var("c", exp_c));
                    utt_assert.equal(exp_c.data_ref.as_string(), std::string("rel0"));

                    // Access global a from child scope
                    scope::exported_ref exp_a_child;
                    utt_assert.is_true(child.variables().export_var("a", exp_a_child));
                    utt_assert.equal(exp_a_child.data_ref.as_string(), std::string("abs0"));

                    // Define local shadows or further variables
                    utt_assert.is_true(child.variables().define("d", scope::type_t::ptr_type));
                    scope::exported_ref exp_c_after;
                    utt_assert.is_true(child.variables().export_var("c", exp_c_after));
                    utt_assert.equal(exp_c_after.data_ref.as_string(), std::string("rel1"));

                    scope::exported_ref exp_d;
                    utt_assert.is_true(child.variables().export_var("d", exp_d));
                    utt_assert.equal(exp_d.data_ref.as_string(), std::string("rel0"));

                    // Grandchild scope
                    {
                        scope grandchild(&child);
                        utt_assert.is_true(grandchild.variables().define("e", scope::type_t::variable_type));
                        scope::exported_ref exp_e;
                        utt_assert.is_true(grandchild.variables().export_var("e", exp_e));
                        utt_assert.equal(exp_e.data_ref.as_string(), std::string("rel0"));

                        scope::exported_ref exp_c_gc;
                        utt_assert.is_true(grandchild.variables().export_var("c", exp_c_gc));
                        utt_assert.equal(exp_c_gc.data_ref.as_string(), std::string("rel2"));
                    }
                }
            }

            return true;
        }

        DEFINE_CASE(scope_test);
    };

    REGISTER_CASE(scope_test);
}
