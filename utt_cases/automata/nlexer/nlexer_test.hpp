#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include "../../../automata/nlexer/nlexer.hpp"
#include "../../../utt/icase.hpp"

class nlexer_test : public icase
{
private:
    const std::vector<std::string> rules = {
        "return return",
        "name [\\w,_][\\w,\\d,_]*",
        "left-bracket (",
        "right-bracket )",
        "left-brace {",
        "right-brace }",
        "assignment =",
        "equal ==",
        "raw-string \"[\\\",*|\"]*\"",
        "float [+,-]?[\\d]*.[\\d]+",
        "int [+,-]?[\\d]+",
        "add +",
        "comma \\,",
        "semi-colon ;",
        "space [\\b]+"
    };

    const std::vector<std::string> program = {
        "void function_a(type1 param1, type2 param2) {",
        "  string s = \"a bc\\\"\";",
        "  double x = -1.9;",
        "  int y = +100;",
        "  return param1 + param2 + s;",
        "}"
    };

    bool test_original_result()
    {
        using namespace osi::automata::nlexer;
        std::shared_ptr<nlexer> r;
        if (!utt_assert.is_true(nlexer::of(rules, r))) return false;
        auto v = r->match(program);
        if (!utt_assert.is_true(v.has_value())) return false;
        const auto& res = *v;
        std::vector<result> expected = {
            r->result_of(0, 4, "name"),
            r->result_of(4, 5, "space"),
            r->result_of(5, 15, "name"),
            r->result_of(15, 16, "left-bracket"),
            r->result_of(16, 21, "name"),
            r->result_of(21, 22, "space"),
            r->result_of(22, 28, "name"),
            r->result_of(28, 29, "comma"),
            r->result_of(29, 30, "space"),
            r->result_of(30, 35, "name"),
            r->result_of(35, 36, "space"),
            r->result_of(36, 42, "name"),
            r->result_of(42, 43, "right-bracket"),
            r->result_of(43, 44, "space"),
            r->result_of(44, 45, "left-brace"),
            r->result_of(45, 48, "space"),
            r->result_of(48, 54, "name"),
            r->result_of(54, 55, "space"),
            r->result_of(55, 56, "name"),
            r->result_of(56, 57, "space"),
            r->result_of(57, 58, "assignment"),
            r->result_of(58, 59, "space"),
            r->result_of(59, 67, "raw-string"),
            r->result_of(67, 68, "semi-colon"),
            r->result_of(68, 71, "space"),
            r->result_of(71, 77, "name"),
            r->result_of(77, 78, "space"),
            r->result_of(78, 79, "name"),
            r->result_of(79, 80, "space"),
            r->result_of(80, 81, "assignment"),
            r->result_of(81, 82, "space"),
            r->result_of(82, 86, "float"),
            r->result_of(86, 87, "semi-colon"),
            r->result_of(87, 90, "space"),
            r->result_of(90, 93, "name"),
            r->result_of(93, 94, "space"),
            r->result_of(94, 95, "name"),
            r->result_of(95, 96, "space"),
            r->result_of(96, 97, "assignment"),
            r->result_of(97, 98, "space"),
            r->result_of(98, 102, "int"),
            r->result_of(102, 103, "semi-colon"),
            r->result_of(103, 106, "space"),
            r->result_of(106, 112, "return"),
            r->result_of(112, 113, "space"),
            r->result_of(113, 119, "name"),
            r->result_of(119, 120, "space"),
            r->result_of(120, 121, "add"),
            r->result_of(121, 122, "space"),
            r->result_of(122, 128, "name"),
            r->result_of(128, 129, "space"),
            r->result_of(129, 130, "add"),
            r->result_of(130, 131, "space"),
            r->result_of(131, 132, "name"),
            r->result_of(132, 133, "semi-colon"),
            r->result_of(133, 134, "space"),
            r->result_of(134, 135, "right-brace")
        };
        if (!utt_assert.equal(res.size(), expected.size())) return false;
        for (size_t i = 0; i < res.size(); ++i)
        {
            if (!utt_assert.equal(res[i].to_string(), expected[i].to_string())) return false;
        }
        return true;
    }

    bool test_filtered_result()
    {
        using namespace osi::automata::nlexer;
        std::shared_ptr<nlexer> r;
        if (!utt_assert.is_true(nlexer::of(rules, r))) return false;
        auto v = r->match(program, {"space"});
        if (!utt_assert.is_true(v.has_value())) return false;
        const auto& res = *v;
        std::vector<result> expected = {
            r->result_of(0, 4, "name"),
            r->result_of(5, 15, "name"),
            r->result_of(15, 16, "left-bracket"),
            r->result_of(16, 21, "name"),
            r->result_of(22, 28, "name"),
            r->result_of(28, 29, "comma"),
            r->result_of(30, 35, "name"),
            r->result_of(36, 42, "name"),
            r->result_of(42, 43, "right-bracket"),
            r->result_of(44, 45, "left-brace"),
            r->result_of(48, 54, "name"),
            r->result_of(55, 56, "name"),
            r->result_of(57, 58, "assignment"),
            r->result_of(59, 67, "raw-string"),
            r->result_of(67, 68, "semi-colon"),
            r->result_of(71, 77, "name"),
            r->result_of(78, 79, "name"),
            r->result_of(80, 81, "assignment"),
            r->result_of(82, 86, "float"),
            r->result_of(86, 87, "semi-colon"),
            r->result_of(90, 93, "name"),
            r->result_of(94, 95, "name"),
            r->result_of(96, 97, "assignment"),
            r->result_of(98, 102, "int"),
            r->result_of(102, 103, "semi-colon"),
            r->result_of(106, 112, "return"),
            r->result_of(113, 119, "name"),
            r->result_of(120, 121, "add"),
            r->result_of(122, 128, "name"),
            r->result_of(129, 130, "add"),
            r->result_of(131, 132, "name"),
            r->result_of(132, 133, "semi-colon"),
            r->result_of(134, 135, "right-brace")
        };
        if (!utt_assert.equal(res.size(), expected.size())) return false;
        for (size_t i = 0; i < res.size(); ++i)
        {
            if (!utt_assert.equal(res[i].to_string(), expected[i].to_string())) return false;
        }
        return true;
    }

    bool test_str_results()
    {
        using namespace osi::automata::nlexer;
        std::shared_ptr<nlexer> r;
        if (!utt_assert.is_true(nlexer::of(rules, r))) return false;
        auto v = r->match(program, {"space"});
        if (!utt_assert.is_true(v.has_value())) return false;
        auto sv = str_result::of(program, *v);
        std::vector<str_result> expected = {
            str_result("void", "name"),
            str_result("function_a", "name"),
            str_result("(", "left-bracket"),
            str_result("type1", "name"),
            str_result("param1", "name"),
            str_result(",", "comma"),
            str_result("type2", "name"),
            str_result("param2", "name"),
            str_result(")", "right-bracket"),
            str_result("{", "left-brace"),
            str_result("string", "name"),
            str_result("s", "name"),
            str_result("=", "assignment"),
            str_result("\"a bc\\\"\"", "raw-string"),
            str_result(";", "semi-colon"),
            str_result("double", "name"),
            str_result("x", "name"),
            str_result("=", "assignment"),
            str_result("-1.9", "float"),
            str_result(";", "semi-colon"),
            str_result("int", "name"),
            str_result("y", "name"),
            str_result("=", "assignment"),
            str_result("+100", "int"),
            str_result(";", "semi-colon"),
            str_result("return", "return"),
            str_result("param1", "name"),
            str_result("+", "add"),
            str_result("param2", "name"),
            str_result("+", "add"),
            str_result("s", "name"),
            str_result(";", "semi-colon"),
            str_result("}", "right-brace")
        };
        if (!utt_assert.equal(sv.size(), expected.size())) return false;
        for (size_t i = 0; i < sv.size(); ++i)
        {
            if (!utt_assert.equal(sv[i].to_string(), expected[i].to_string())) return false;
        }
        return true;
    }

    bool test_longest_match()
    {
        using namespace osi::automata::nlexer;
        std::mt19937 g(42);
        for (int round = 0; round < 100; ++round)
        {
            std::vector<std::string> shuffled = rules;
            std::shuffle(shuffled.begin(), shuffled.end(), g);
            auto it = std::find(shuffled.begin(), shuffled.end(), "return return");
            if (!utt_assert.is_true(it != shuffled.end())) return false;
            std::swap(shuffled[0], *it);

            std::shared_ptr<nlexer> r;
            if (!utt_assert.is_true(nlexer::of(shuffled, r))) return false;
            auto v = r->match(program, {"space"});
            if (!utt_assert.is_true(v.has_value())) return false;
            auto sv = str_result::of(program, *v);
            if (!utt_assert.equal(sv.size(), size_t(33))) return false;
            if (!utt_assert.equal(sv[0].to_string(), str_result("void", "name").to_string())) return false;
            if (!utt_assert.equal(sv[25].to_string(), str_result("return", "return").to_string())) return false;
        }
        return true;
    }

public:
    bool run() override
    {
        return test_original_result() &&
               test_filtered_result() &&
               test_str_results() &&
               test_longest_match();
    }

    DEFINE_CASE(nlexer_test);
};

REGISTER_CASE(nlexer_test);
