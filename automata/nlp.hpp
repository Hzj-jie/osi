#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../app_info/assert.hpp"
#include "lang_parser.hpp"
#include "nlexer/nlexer.hpp"
#include "syntax/syntaxer.hpp"
#include "syntax/rule.hpp"

namespace osi
{
    namespace automata
    {
        class nlp : public lang_parser
        {
        private:
            std::shared_ptr<nlexer::nlexer> n_;
            std::shared_ptr<syntax::syntaxer> s_;

        public:
            nlp(std::shared_ptr<nlexer::nlexer> n, std::shared_ptr<syntax::syntaxer> s)
                : n_(std::move(n)), s_(std::move(s))
            {
                assert(n_ != nullptr);
                assert(s_ != nullptr);
            }

            bool parse(const std::string& txt,
                       std::vector<typed_word>& words,
                       std::shared_ptr<typed_node>& root) override
            {
                return n_->match(txt, words) && s_->match(words, root);
            }

            using lang_parser::parse;

            const std::shared_ptr<nlexer::nlexer>& lexer() const { return n_; }
            const std::shared_ptr<syntax::syntaxer>& syn() const { return s_; }

            static bool of(std::shared_ptr<nlexer::nlexer> n,
                           std::shared_ptr<syntax::syntaxer> s,
                           std::shared_ptr<nlp>& o)
            {
                if (n == nullptr || s == nullptr)
                {
                    return false;
                }
                o = std::make_shared<nlp>(std::move(n), std::move(s));
                return true;
            }

            static bool of(const std::string& n_content,
                           const std::string& s_content,
                           std::shared_ptr<nlp>& o)
            {
                std::shared_ptr<nlexer::nlexer> n;
                if (!nlexer::nlexer::of(n_content, n))
                {
                    return false;
                }
                syntax::rule r(n->str_type_mapping());
                if (!r.parse_content(s_content))
                {
                    return false;
                }
                return of(n, r.export_syntaxer().syntaxer(), o);
            }

            static std::shared_ptr<nlp> of(const std::string& n_content, const std::string& s_content)
            {
                std::shared_ptr<nlp> o;
                assert(of(n_content, s_content, o));
                return o;
            }

            static bool of(const std::vector<std::string>& n_lines,
                           const std::vector<std::string>& s_lines,
                           std::shared_ptr<nlp>& o)
            {
                std::shared_ptr<nlexer::nlexer> n;
                if (!nlexer::nlexer::of(n_lines, n))
                {
                    return false;
                }
                syntax::rule r(n->str_type_mapping());
                if (!r.parse(s_lines))
                {
                    return false;
                }
                return of(n, r.export_syntaxer().syntaxer(), o);
            }

            static std::shared_ptr<nlp> of(const std::vector<std::string>& n_lines,
                                           const std::vector<std::string>& s_lines)
            {
                std::shared_ptr<nlp> o;
                assert(of(n_lines, s_lines, o));
                return o;
            }

            static bool of_file(const std::string& n_file,
                                const std::string& s_file,
                                std::shared_ptr<nlp>& o)
            {
                std::shared_ptr<nlexer::nlexer> n;
                if (!nlexer::nlexer::of_file(n_file, n))
                {
                    return false;
                }
                syntax::rule r(n->str_type_mapping());
                if (!r.parse_file(s_file))
                {
                    return false;
                }
                return of(n, r.export_syntaxer().syntaxer(), o);
            }

            static std::shared_ptr<nlp> of_file(const std::string& n_file, const std::string& s_file)
            {
                std::shared_ptr<nlp> o;
                assert(of_file(n_file, s_file, o));
                return o;
            }
        };
    }
}
