#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include "../../../automata/syntax/syntaxer.hpp"
#include "../../../automata/syntax/rule.hpp"

using namespace osi::automata;

void test_direct_syntaxer()
{
    std::unordered_map<std::string, uint32_t> tokens = {{"NUM", 0}, {"PLUS", 1}, {"SPACE", 2}};
    syntax_collection sc(tokens);

    syntax::rule r(sc);
    std::vector<std::string> rule_lines = {
        "IGNORE_TYPES SPACE",
        "ROOT_TYPES expr",
        "expr NUM [plus_expr]?",
        "plus_expr PLUS NUM"
    };
    assert(r.parse(rule_lines));
    auto exporter = r.export_syntaxer();
    auto syn = exporter.syntaxer();
    assert(syn != nullptr);

    // Create typed_words: NUM PLUS NUM
    std::string text = "1 + 2";
    std::vector<typed_word> words = {
        typed_word(text, 0, 1, 0, "NUM"),
        typed_word(text, 2, 3, 1, "PLUS"),
        typed_word(text, 4, 5, 0, "NUM")
    };

    std::shared_ptr<typed_node> root;
    assert(syn->match(words, root));
    assert(root != nullptr);
    assert(!root->leaf());
    assert(root->subnodes.size() == 1);
    assert(root->child(0)->type_name == "expr");

    std::cout << "[PASS] test_direct_syntaxer\n";
}

void test_cycle_dependency()
{
    std::unordered_map<std::string, uint32_t> tokens = {{"self-inc", 0}, {"variable-name", 1}};
    syntax_collection sc(tokens);

    syntax::rule r(sc);
    std::vector<std::string> rule_lines = {
        "ROOT_TYPES value",
        "value [pre-operation-value, post-operation-value, variable-name]",
        "pre-operation-value self-inc value",
        "post-operation-value value self-inc"
    };
    assert(r.parse(rule_lines));
    auto exporter = r.export_syntaxer();
    auto syn = exporter.syntaxer();
    assert(syn != nullptr);

    // Test matching a simple variable-name
    std::string text = "x";
    std::vector<typed_word> words = {
        typed_word(text, 0, 1, 1, "variable-name")
    };
    std::shared_ptr<typed_node> root;
    assert(syn->match(words, root));
    assert(root != nullptr);
    assert(root->child(0)->type_name == "value");

    std::cout << "[PASS] test_cycle_dependency\n";
}

int main()
{
    test_direct_syntaxer();
    test_cycle_dependency();
    std::cout << "All syntaxer tests passed!\n";
    return 0;
}
