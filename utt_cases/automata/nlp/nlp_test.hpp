#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../../../automata/nlp.hpp"
#include "../../../utt/icase.hpp"

class nlp_test : public icase
{
private:
    const std::string nlexer_rule = R"(
KW-if if
KW-else else
KW-for for
KW-while while
KW-do do
KW-loop loop
KW-return return
KW-break break

blank [\b]+

bool [true,false]
integer [+,-]?[\d]+
float [+,-]?[\d]*.[\d]+
string "[\",*|"]*"

less-or-equal <=
great-or-equal >=
equal ==
less-than <
great-than >
assignment =
comma \,
colon :
question-mark ?
start-paragraph {
end-paragraph }
start-bracket (
end-bracket )
start-square-bracket \[
end-square-bracket \] 
semi-colon ;

name [\w,_][\w,\d,_]*
)";

    const std::string syntaxer_rule = R"(
IGNORE_TYPES blank
ROOT_TYPES function

function name name start-bracket [paramlist]? end-bracket multi-sentence-paragraph
paramlist [param-with-comma]* param
param-with-comma param comma
param name name
paragraph [sentence, multi-sentence-paragraph]
sentence [sentence-with-semi-colon, sentence-without-semi-colon, semi-colon]
sentence-with-semi-colon [value-definition, value-clause, return-clause, function-call, KW-break] semi-colon
sentence-without-semi-colon [condition]
multi-sentence-paragraph start-paragraph [sentence]* end-paragraph
value-definition name name
value-clause value assignment value
condition KW-if start-bracket value end-bracket paragraph [else-condition]?
else-condition KW-else paragraph
value [comparasion, function-call, name, integer, float, bool, string]
comparasion value-without-comparasion [less-than, great-than, less-or-equal, great-or-equal, equal] value
value-without-comparasion [function-call, name, integer, float, bool, string]
function-call name start-bracket [value-list]? end-bracket
value-list [value-with-comma]* value
value-with-comma value comma
return-clause KW-return [value]?
)";

    struct tree_node
    {
        std::string this_name;
        std::vector<tree_node> subnodes;

        tree_node(std::string name, std::vector<tree_node> subs)
            : this_name(std::move(name)), subnodes(std::move(subs)) {}
        explicit tree_node(std::string name)
            : this_name(std::move(name)) {}
    };

    static tree_node make_root(std::vector<tree_node> subs)
    {
        return tree_node("ROOT", std::move(subs));
    }

    bool assert_node(const std::shared_ptr<osi::automata::typed_node>& n, const tree_node& t)
    {
        if (!utt_assert.is_not_null(n.get(), "Node is null"))
            return false;

        if (t.this_name == "ROOT")
        {
            if (!utt_assert.is_true(n->root(), "Expected ROOT, got ", n->type_name))
                return false;
        }
        else
        {
            if (!utt_assert.equal(n->type_name, t.this_name, "Expected ", t.this_name, ", got ", n->type_name))
                return false;
        }

        if (!utt_assert.equal(n->subnodes.size(), t.subnodes.size(),
                              "Subnodes count mismatch for ", n->type_name))
            return false;

        for (size_t i = 0; i < t.subnodes.size(); ++i)
        {
            if (!assert_node(n->child(i), t.subnodes[i]))
                return false;
        }
        return true;
    }

    bool test_parsable(const std::shared_ptr<osi::automata::nlp>& parser)
    {
        return utt_assert.is_not_null(parser.get());
    }

    bool test_case1(const std::shared_ptr<osi::automata::nlp>& parser)
    {
        using namespace osi::automata;
        const std::string code = "void main(string args) { return 0; }";
        std::shared_ptr<typed_node> root;
        bool ok = parser->parse(code, root);
        if (!utt_assert.is_true(ok)) return false;
        if (!utt_assert.is_not_null(root.get())) return false;

        tree_node expected = make_root({
            tree_node("function", {
                tree_node("name"),
                tree_node("name"),
                tree_node("start-bracket"),
                tree_node("paramlist", {
                    tree_node("param", {
                        tree_node("name"),
                        tree_node("name")
                    })
                }),
                tree_node("end-bracket"),
                tree_node("multi-sentence-paragraph", {
                    tree_node("start-paragraph"),
                    tree_node("sentence", {
                        tree_node("sentence-with-semi-colon", {
                            tree_node("return-clause", {
                                tree_node("KW-return"),
                                tree_node("value", {
                                    tree_node("integer")
                                })
                            }),
                            tree_node("semi-colon")
                        })
                    }),
                    tree_node("end-paragraph")
                })
            })
        });

        return assert_node(root, expected);
    }

    bool test_case2(const std::shared_ptr<osi::automata::nlp>& parser)
    {
        using namespace osi::automata;
        const std::string code = "int max(int x, int y) { \n"
                                 "if (x > y) return x;\n"
                                 "else return y ;  }";
        std::shared_ptr<typed_node> root;
        bool ok = parser->parse(code, root);
        if (!utt_assert.is_true(ok)) return false;
        if (!utt_assert.is_not_null(root.get())) return false;

        tree_node expected = make_root({
            tree_node("function", {
                tree_node("name"),
                tree_node("name"),
                tree_node("start-bracket"),
                tree_node("paramlist", {
                    tree_node("param-with-comma", {
                        tree_node("param", {
                            tree_node("name"),
                            tree_node("name")
                        }),
                        tree_node("comma")
                    }),
                    tree_node("param", {
                        tree_node("name"),
                        tree_node("name")
                    })
                }),
                tree_node("end-bracket"),
                tree_node("multi-sentence-paragraph", {
                    tree_node("start-paragraph"),
                    tree_node("sentence", {
                        tree_node("sentence-without-semi-colon", {
                            tree_node("condition", {
                                tree_node("KW-if"),
                                tree_node("start-bracket"),
                                tree_node("value", {
                                    tree_node("comparasion", {
                                        tree_node("value-without-comparasion", {
                                            tree_node("name")
                                        }),
                                        tree_node("great-than"),
                                        tree_node("value", {
                                            tree_node("name")
                                        })
                                    })
                                }),
                                tree_node("end-bracket"),
                                tree_node("paragraph", {
                                    tree_node("sentence", {
                                        tree_node("sentence-with-semi-colon", {
                                            tree_node("return-clause", {
                                                tree_node("KW-return"),
                                                tree_node("value", {
                                                    tree_node("name")
                                                })
                                            }),
                                            tree_node("semi-colon")
                                        })
                                    })
                                }),
                                tree_node("else-condition", {
                                    tree_node("KW-else"),
                                    tree_node("paragraph", {
                                        tree_node("sentence", {
                                            tree_node("sentence-with-semi-colon", {
                                                tree_node("return-clause", {
                                                    tree_node("KW-return"),
                                                    tree_node("value", {
                                                        tree_node("name")
                                                    })
                                                }),
                                                tree_node("semi-colon")
                                            })
                                        })
                                    })
                                })
                            })
                        })
                    }),
                    tree_node("end-paragraph")
                })
            })
        });

        return assert_node(root, expected);
    }

    bool test_case3(const std::shared_ptr<osi::automata::nlp>& parser)
    {
        using namespace osi::automata;
        const std::string code = "int f1(int x, int y) {\n"
                                 "if( x > 0) return x;\n"
                                 "else return y; }\n"
                                 "int f2()\n"
                                 "{\n"
                                 "    return f1(100,10);\n"
                                 "}";
        std::shared_ptr<typed_node> root;
        bool ok = parser->parse(code, root);
        if (!utt_assert.is_true(ok)) return false;
        if (!utt_assert.is_not_null(root.get())) return false;
        return utt_assert.equal(root->subnodes.size(), size_t(2));
    }

    bool test_escape_str(const std::shared_ptr<osi::automata::nlp>& parser)
    {
        using namespace osi::automata;
        const std::string code = "string main() { return \"\\\"\"; }";
        std::shared_ptr<typed_node> root;
        bool ok = parser->parse(code, root);
        if (!utt_assert.is_true(ok)) return false;
        return utt_assert.is_not_null(root.get());
    }

    bool test_case0_and_case1_files(const std::shared_ptr<osi::automata::nlp>& parser)
    {
        using namespace osi::automata;
        const std::string case0 = R"(
void main(string args) { return 0; }

int main(int x, int y) {
if (x > y) reutrn x;
else return y ;  }

int f1(int x, int y) {
if( x > 0) return x;
else return y; }

int f2()
{
    return f1(100, 10);
}
)";

        std::shared_ptr<typed_node> root0;
        if (!utt_assert.is_true(parser->parse(case0, root0))) return false;
        if (!utt_assert.is_not_null(root0.get())) return false;

        const std::string case1 = R"(
string func1(string x,  string  y) {
    if (strcmp(x, "abc") == 0) {
        return x;
    } else {
        return y;
    }
}

int blabla(int start, int end)
{
    int x;
    int y;
    x = 100;
    y=1.1;
}

int max(int a, int b)
{
    if (a > b) return a;
    else return b;
}

int max(int a, int b, int c)
{
    if (a > b)
    {
        if(a > c)
        {
            return a;
        } else
        {
            return c;
        }
    }
    else
    {
        if (b>c)
            return b;
        else return c;
    }
}

int max(int a, int b, int c, int d)
{
    return max(max(a,b,c),d);
}

int blabla2()
{
    return max(1, 2, 3.3, 1991919);
}

int case_insensitive_compare(string a, string b)
{
    return strcasecmp(a, b);
}

int main()
{
    func1(1.1, -1.23, +1.32992);
    if (1.1 > -1.1)
    {
        printf("%llf", 1.1);
    } else printf("%llf", -1.1);
}

double func1(int x, int y, double z)
{
    if (x <= 1.11) {
        if (y < 2.22) {
            if (z >= 2.23) return z;
        } else return 2.322;
    } else { return 1.11;}
}
)";

        std::shared_ptr<typed_node> root1;
        if (!utt_assert.is_true(parser->parse(case1, root1))) return false;
        if (!utt_assert.is_not_null(root1.get())) return false;

        return true;
    }

public:
    bool run() override
    {
        using namespace osi::automata;
        std::shared_ptr<nlp> parser;
        if (!utt_assert.is_true(nlp::of(nlexer_rule, syntaxer_rule, parser))) return false;
        if (!utt_assert.is_not_null(parser.get())) return false;

        return test_parsable(parser) &&
               test_case1(parser) &&
               test_case2(parser) &&
               test_case3(parser) &&
               test_escape_str(parser) &&
               test_case0_and_case1_files(parser);
    }

    DEFINE_CASE(nlp_test);
};

REGISTER_CASE(nlp_test);
