#pragma once
#include <string>
#include "binary_operator.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class compare : public binary_operator
            {
            protected:
                compare(std::string result, std::string left, std::string right)
                    : binary_operator(std::move(result), std::move(left), std::move(right)) {}

                bool result_restrict(const variable& result) const override
                {
                    return result.is_assignable_from_bool();
                }

                bool parameter_restrict(const variable&) const override
                {
                    return true;
                }
            };

            class compare_or_equal : public instruction_gen
            {
            protected:
                std::string result;
                std::string left;
                std::string right;

                compare_or_equal(std::string result, std::string left, std::string right)
                    : result(std::move(result)), left(std::move(left)), right(std::move(right))
                {
                    assert(!this->result.empty());
                    assert(!this->left.empty());
                    assert(!this->right.empty());
                }

                virtual std::string compare_cmd() const = 0;
                virtual std::string equal_cmd() const { return "equal"; }

            public:
                bool build(std::vector<std::string>& o) const override
                {
                    variable result_var, left_var, right_var;
                    if (!variable::of(result, o, result_var) || !result_var.is_assignable_from_bool())
                    {
                        return false;
                    }
                    if (!variable::of(left, o, left_var)) return false;
                    if (!variable::of(right, o, right_var)) return false;

                    o.push_back(strcat(compare_cmd(), " ", result_var.ToString(), " ", left_var.ToString(), " ", right_var.ToString()));
                    o.push_back(strcat("jumpif rel2 ", result_var.ToString()));
                    o.push_back(strcat(equal_cmd(), " ", result_var.ToString(), " ", left_var.ToString(), " ", right_var.ToString()));
                    return true;
                }
            };
        }
    }
}
