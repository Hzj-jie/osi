#pragma once
#include <memory>
#include "../lazy_list_writer.hpp"
#include "../../automata/typed_node.hpp"

namespace osi
{
    namespace compiler
    {
        namespace rewriter
        {
            class typed_node_writer : public lazy_list_writer
            {
            public:
                using lazy_list_writer::append;

                bool append(const std::shared_ptr<automata::typed_node>& t)
                {
                    assert(t != nullptr);
                    return lazy_list_writer::append([t]() { return t->input(); });
                }

                bool append(const automata::typed_node* t)
                {
                    assert(t != nullptr);
                    return lazy_list_writer::append([t]() { return t->input(); });
                }
            };
        }
    }
}
