#pragma once
#include <string>
#include <memory>
#include <functional>
#include "../../app_info/assert.hpp"
#include "../../automata/typed_node.hpp"
#include "../code_gen/code_gen.hpp"
#include "typed_node_writer.hpp"

namespace osi
{
    namespace compiler
    {
        namespace rewriter
        {
            class leaf : public code_gen<typed_node_writer>
            {
            public:
                static std::function<void(code_gens<typed_node_writer>&)> of(const std::string& name)
                {
                    assert(!name.empty());
                    return [name](code_gens<typed_node_writer>& cg) {
                        cg.register_gen(name, std::make_shared<leaf>());
                    };
                }

                bool build(const std::shared_ptr<automata::typed_node>& n,
                           typed_node_writer& o) override
                {
                    assert(n != nullptr && n->leaf());
                    return o.append(n);
                }
            };
        }
    }
}
