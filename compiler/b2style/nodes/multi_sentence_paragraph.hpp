#pragma once
#include <memory>
#include "../scope.hpp"
#include "../b2style_forward.hpp"
#include "../../rewriter/typed_node_writer.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {
            class multi_sentence_paragraph_node : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "multi-sentence-paragraph"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() >= 2);
                    assert(n->child(0)->type_name == "start-paragraph");
                    assert(n->last_child()->type_name == "end-paragraph");

                    if (!o.append("{"))
                    {
                        return false;
                    }

                    {
                        scope child_scope(scope::current());
                        for (uint32_t i = 1; i + 1 < n->child_count(); ++i)
                        {
                            if (!code_gen_of(n->child(i)).build(o))
                            {
                                return false;
                            }
                        }
                        child_scope.run_end_scope();
                    }

                    return o.append("}");
                }
            };
        }
    }
}
