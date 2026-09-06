#pragma once
#include <string>
#include <vector>
#include <memory>
#include "typed_word.hpp"
#include "typed_node.hpp"

namespace osi
{
    namespace automata
    {
        class lang_parser
        {
        public:
            virtual ~lang_parser() = default;
            virtual bool parse(const std::string& txt,
                               std::vector<typed_word>& words,
                               std::shared_ptr<typed_node>& root) = 0;

            bool parse(const std::string& txt, std::shared_ptr<typed_node>& root)
            {
                std::vector<typed_word> words;
                return parse(txt, words, root);
            }

            bool parse(const std::string& txt)
            {
                std::vector<typed_word> words;
                std::shared_ptr<typed_node> root;
                return parse(txt, words, root);
            }
        };
    }
}
