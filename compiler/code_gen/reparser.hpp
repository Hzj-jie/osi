#pragma once
#include <string>
#include <memory>
#include "code_gen.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../../automata/typed_node.hpp"

namespace osi
{
    namespace compiler
    {
        template <typename Writer>
        class reparser : public code_gen<Writer>
        {
        public:
            bool build(const std::shared_ptr<automata::typed_node>& n, Writer& o) override
            {
                assert(n != nullptr);
                std::string s;
                if (!dump(n, s))
                {
                    raise_error("Failed to dump node ", n->input());
                    return false;
                }
                if (!parse(s, o))
                {
                    raise_error("Failed to parse node ", n->input());
                    return false;
                }
                return true;
            }

        protected:
            virtual bool dump(const std::shared_ptr<automata::typed_node>& n, std::string& s) = 0;
            virtual bool parse(const std::string& s, Writer& o) = 0;
        };
    }
}
