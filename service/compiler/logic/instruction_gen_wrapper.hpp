#pragma once
#include <string>
#include <vector>
#include <memory>
#include "instruction_gen.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class instruction_gen_wrapper : public instruction_gen
            {
            private:
                std::shared_ptr<instruction_gen> e;

            public:
                explicit instruction_gen_wrapper(std::shared_ptr<instruction_gen> e)
                    : e(std::move(e)) {}

                static std::shared_ptr<instruction_gen> maybe_wrap(
                    const std::vector<std::string>& /*tokens*/,
                    size_t /*start*/,
                    size_t /*end*/,
                    std::shared_ptr<instruction_gen> e)
                {
                    return e;
                }

                bool build(std::vector<std::string>& o) const override
                {
                    return e && e->build(o);
                }
            };
        }
    }
}
