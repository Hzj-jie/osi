#pragma once
#include <functional>
#include <string>
#include "../lazy_list_writer.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class logic_writer : public lazy_list_writer
            {
            public:
                using lazy_list_writer::append;

                logic_writer() = default;

                bool append(const std::function<bool(logic_writer&)>& a)
                {
                    assert(a != nullptr);
                    return a(*this);
                }

                std::string dump() const
                {
                    return str();
                }
            };
        }

        using logic_writer = logic::logic_writer;
    }
}
