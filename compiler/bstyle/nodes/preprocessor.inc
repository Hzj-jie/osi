#ifndef DIALECT
#error "DIALECT must be defined before including preprocessor.inc"
#endif

#ifndef WRITER
#define WRITER logic_writer
#endif

namespace osi
{
    namespace compiler
    {
        namespace DIALECT
        {
            class if_wrapped : public code_gen<WRITER>
            {
            private:
                std::function<bool(const std::string&)> bypass_;

            public:
                explicit if_wrapped(std::function<bool(const std::string&)> bypass)
                    : bypass_(std::move(bypass))
                {
                    assert(bypass_ != nullptr);
                }

                static std::shared_ptr<if_wrapped> ifdef_wrapped()
                {
                    return std::make_shared<if_wrapped>([](const std::string& s) {
                        return !scope::current()->defines().is_defined(s);
                    });
                }

                static std::shared_ptr<if_wrapped> ifndef_wrapped()
                {
                    return std::make_shared<if_wrapped>([](const std::string& s) {
                        return scope::current()->defines().is_defined(s);
                    });
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, WRITER& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() >= 3);
                    auto defines_node = n->child(1);
                    for (uint32_t i = 0; i < defines_node->child_count(); ++i)
                    {
                        std::string s;
                        if (i == defines_node->child_count() - 1)
                        {
                            s = defines_node->last_child()->word().str();
                        }
                        else
                        {
                            s = defines_node->child(i)->child(0)->word().str();
                        }
                        if (!bypass_(s))
                        {
                            for (uint32_t j = 2; j <= n->child_count() - 3; ++j)
                            {
                                if (!code_gen_of(n->child(j)).build(o))
                                {
                                    return false;
                                }
                            }
                            auto second_last = n->child(n->child_count() - 2);
                            return (second_last->type_name == "delse-wrapped") ||
                                   code_gen_of(second_last).build(o);
                        }
                    }
                    auto second_last = n->child(n->child_count() - 2);
                    if (second_last->type_name == "delse-wrapped")
                    {
                        for (uint32_t j = 1; j < second_last->child_count(); ++j)
                        {
                            if (!code_gen_of(second_last->child(j)).build(o))
                            {
                                return false;
                            }
                        }
                        return true;
                    }
                    return true;
                }
            };

            class define_node : public code_gen<WRITER>
            {
            public:
                static const char* node_name() { return "define"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, WRITER&) override
                {
                    assert(n != nullptr && n->child_count() == 2);
                    scope::current()->defines().define(n->child(1)->word().str());
                    return true;
                }
            };

#ifdef PREPROCESSOR_FOLDERS
            struct should_include
            {
                bool operator()(const std::string& s) const
                {
                    return scope::current()->includes().should_include(s);
                }
            };

            inline bool parse_include_file(const std::string& file, WRITER& o)
            {
                if (file.empty())
                {
                    return true;
                }
                std::ifstream ifs(file);
                if (!ifs.is_open())
                {
                    raise_error("Cannot read content from ", file);
                    return false;
                }
                std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                current_file_guard guard(file);
                return build_code(content, o);
            }

            class include_with_string_node : public include_with_string<WRITER, should_include, PREPROCESSOR_FOLDERS>
            {
            public:
                static const char* node_name() { return "include-with-string"; }

            protected:
                bool file_parse(const std::string& s, WRITER& o) override
                {
                    return parse_include_file(s, o);
                }
            };

            class include_with_file_node : public include_with_file<WRITER, should_include, PREPROCESSOR_FOLDERS>
            {
            public:
                static const char* node_name() { return "include-with-file"; }

            protected:
                bool file_parse(const std::string& s, WRITER& o) override
                {
                    return parse_include_file(s, o);
                }
            };
#endif
        }
    }
}
