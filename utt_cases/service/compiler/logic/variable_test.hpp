#pragma once
#include "../../../../service/compiler/logic/scope.hpp"
#include "../../../../service/compiler/logic/variable.hpp"
#include "../../../../utt/icase.hpp"
#include "../../../../utt/utt_assert.hpp"

namespace logic_test
{
    class variable_test : public icase
    {
    public:
        bool run() override
        {
            using namespace osi::compiler::logic;

            // 1. Primitive variable
            {
                std::vector<std::string> v;
                scope s;
                utt_assert.is_true(s.variables().define("abc", scope::type_t::variable_type));
                variable o;
                utt_assert.is_true(variable::of("abc", v, o));
                utt_assert.equal(o.ToString(), std::string("abs0"));
                utt_assert.is_true(v.empty());
                utt_assert.equal(o.name, std::string("abc"));
                utt_assert.is_null(o.index.get());
                utt_assert.equal(o.size, scope::type_t::variable_size);
                utt_assert.equal(o.type, scope::type_t::variable_type);
            }

            // 2. Heap variable abc[i]
            {
                std::vector<std::string> v;
                scope s;
                utt_assert.is_true(s.variables().define("abc", scope::type_t::variable_type));
                utt_assert.is_true(s.variables().define("i", scope::type_t::ptr_type));
                variable o;
                utt_assert.is_true(variable::of("abc[i]", v, o));
                utt_assert.equal(o.ToString(), std::string("habs2"));
                utt_assert.equal(v.size(), 2);
                utt_assert.equal(v[0], std::string("push"));
                utt_assert.equal(v[1], std::string("add abs2 abs0 abs1"));
                utt_assert.equal(o.type, scope::type_t::variable_type);
                utt_assert.is_not_null(o.index.get());
                utt_assert.equal(o.index->name, std::string("i"));
                utt_assert.equal(o.index->type, scope::type_t::ptr_type);
            }

            // 3. Nested heap variable abc[def[i]]
            {
                std::vector<std::string> v;
                scope s;
                utt_assert.is_true(s.variables().define("abc", scope::type_t::variable_type));
                utt_assert.is_true(s.variables().define("def", scope::type_t::variable_type));
                utt_assert.is_true(s.variables().define("i", scope::type_t::ptr_type));
                variable o;
                utt_assert.is_true(variable::of("abc[def[i]]", v, o));
                utt_assert.equal(o.ToString(), std::string("habs4"));
                utt_assert.equal(v.size(), 4);
                utt_assert.equal(v[0], std::string("push"));
                utt_assert.equal(v[1], std::string("add abs3 abs1 abs2"));
                utt_assert.equal(v[2], std::string("push"));
                utt_assert.equal(v[3], std::string("add abs4 abs0 habs3"));
            }

            return true;
        }

        DEFINE_CASE(variable_test);
    };

    REGISTER_CASE(variable_test);
}
