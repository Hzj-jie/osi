#pragma once
#include <string>
#include "compare.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class _less : public compare
            {
            public:
                _less(std::string result, std::string left, std::string right)
                    : compare(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "less"; }
            };

            class _more : public compare
            {
            public:
                _more(std::string result, std::string left, std::string right)
                    : compare(std::move(result), std::move(right), std::move(left)) {}
                std::string instruction_name() const override { return "less"; }
            };

            class _equal : public compare
            {
            public:
                _equal(std::string result, std::string left, std::string right)
                    : compare(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "equal"; }
            };

            class _less_or_equal : public compare_or_equal
            {
            public:
                _less_or_equal(std::string result, std::string left, std::string right)
                    : compare_or_equal(std::move(result), std::move(left), std::move(right)) {}
                std::string compare_cmd() const override { return "less"; }
            };

            class _more_or_equal : public compare_or_equal
            {
            public:
                _more_or_equal(std::string result, std::string left, std::string right)
                    : compare_or_equal(std::move(result), std::move(right), std::move(left)) {}
                std::string compare_cmd() const override { return "less"; }
            };

            class _float_less : public compare
            {
            public:
                _float_less(std::string result, std::string left, std::string right)
                    : compare(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "fless"; }
            };

            class _float_more : public compare
            {
            public:
                _float_more(std::string result, std::string left, std::string right)
                    : compare(std::move(result), std::move(right), std::move(left)) {}
                std::string instruction_name() const override { return "fless"; }
            };

            class _float_equal : public compare
            {
            public:
                _float_equal(std::string result, std::string left, std::string right)
                    : compare(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "fequal"; }
            };

            class _float_less_or_equal : public compare_or_equal
            {
            public:
                _float_less_or_equal(std::string result, std::string left, std::string right)
                    : compare_or_equal(std::move(result), std::move(left), std::move(right)) {}
                std::string compare_cmd() const override { return "fless"; }
                std::string equal_cmd() const override { return "fequal"; }
            };

            class _float_more_or_equal : public compare_or_equal
            {
            public:
                _float_more_or_equal(std::string result, std::string left, std::string right)
                    : compare_or_equal(std::move(result), std::move(right), std::move(left)) {}
                std::string compare_cmd() const override { return "fless"; }
                std::string equal_cmd() const override { return "fequal"; }
            };
        }
    }
}
