#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../../../automata/syntax/syntaxer.hpp"
#include "../../../automata/syntax/rule.hpp"
#include "../../../utt/icase.hpp"

class syntaxer_test : public icase
{
private:
    bool test_direct_syntaxer()
    {
        using namespace osi::automata;
        std::unordered_map<std::string, uint32_t> tokens = {{"NUM", 0}, {"PLUS", 1}, {"SPACE", 2}};
        syntax_collection sc(tokens);

        syntax::rule r(sc);
        std::vector<std::string> rule_lines = {
            "IGNORE_TYPES SPACE",
            "ROOT_TYPES expr",
            "expr NUM [plus_expr]?",
            "plus_expr PLUS NUM"
        };
        if (!utt_assert.is_true(r.parse(rule_lines))) return false;
        auto exporter = r.export_syntaxer();
        auto syn = exporter.syntaxer();
        if (!utt_assert.is_not_null(syn.get())) return false;

        // Create typed_words: NUM PLUS NUM
        std::string text = "1 + 2";
        std::vector<typed_word> words = {
            typed_word(text, 0, 1, 0, "NUM"),
            typed_word(text, 2, 3, 1, "PLUS"),
            typed_word(text, 4, 5, 0, "NUM")
        };

        std::shared_ptr<typed_node> root;
        if (!utt_assert.is_true(syn->match(words, root))) return false;
        if (!utt_assert.is_not_null(root.get())) return false;
        if (!utt_assert.is_false(root->leaf())) return false;
        if (!utt_assert.equal(root->subnodes.size(), size_t(1))) return false;
        if (!utt_assert.equal(root->child(0)->type_name, std::string("expr"))) return false;

        return true;
    }

    bool test_cycle_dependency()
    {
        using namespace osi::automata;
        std::unordered_map<std::string, uint32_t> tokens = {{"self-inc", 0}, {"variable-name", 1}};
        syntax_collection sc(tokens);

        syntax::rule r(sc);
        std::vector<std::string> rule_lines = {
            "ROOT_TYPES value",
            "value [pre-operation-value, post-operation-value, variable-name]",
            "pre-operation-value self-inc value",
            "post-operation-value value self-inc"
        };
        if (!utt_assert.is_true(r.parse(rule_lines))) return false;
        auto exporter = r.export_syntaxer();
        auto syn = exporter.syntaxer();
        if (!utt_assert.is_not_null(syn.get())) return false;

        // Test matching a simple variable-name
        std::string text = "x";
        std::vector<typed_word> words = {
            typed_word(text, 0, 1, 1, "variable-name")
        };
        std::shared_ptr<typed_node> root;
        if (!utt_assert.is_true(syn->match(words, root))) return false;
        if (!utt_assert.is_not_null(root.get())) return false;
        if (!utt_assert.equal(root->child(0)->type_name, std::string("value"))) return false;

        return true;
    }

public:
    bool run() override
    {
        return test_direct_syntaxer() &&
               test_cycle_dependency();
    }

    DEFINE_CASE(syntaxer_test);
};

REGISTER_CASE(syntaxer_test);
