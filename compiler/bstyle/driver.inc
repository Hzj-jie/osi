#ifndef DIALECT
#error "DIALECT must be defined before including driver.inc"
#endif
#ifndef WRITER
#error "WRITER must be defined before including driver.inc"
#endif

#define DIALECT_CONCAT_INNER(a, b) a##b
#define DIALECT_CONCAT(a, b) DIALECT_CONCAT_INNER(a, b)
#define DIALECT_NS DIALECT_CONCAT(DIALECT, _compiler)
#define DIALECT_LIB DIALECT_CONCAT(DIALECT, _lib)
#define DIALECT_NLP DIALECT_CONCAT(DIALECT, _nlp)

namespace osi
{
    namespace compiler
    {
        namespace DIALECT_NS
        {
            inline std::shared_ptr<automata::nlp> DIALECT_NLP()
            {
                static std::shared_ptr<automata::nlp> parser = []() {
                    std::shared_ptr<automata::nlp> p;
                    bool ok = automata::nlp::of_file(DIALECT_LIB::nlexer_rule(), DIALECT_LIB::syntaxer_rule(), p);
                    assert(ok && p != nullptr);
                    return p;
                }();
                return parser;
            }

            inline bool build_code(const std::string& input, WRITER& o)
            {
                if (input.empty())
                {
                    return true;
                }
                const std::string* pinput = &input;
                std::string stripped;
                if (input.rfind("\xef\xbb\xbf", 0) == 0)
                {
                    stripped = input.substr(3);
                    pinput = &stripped;
                }
                if (pinput->empty())
                {
                    return true;
                }
                std::shared_ptr<automata::typed_node> root;
                if (!DIALECT_NLP()->parse(*pinput, root))
                {
                    return false;
                }
                assert(root != nullptr);
                assert(root->type == automata::typed_node::root_type);
                assert(root->type_name == automata::typed_node::root_type_name);
                if (root->leaf())
                {
                    return false;
                }
                assert(root->child_count() > 0);
                for (uint32_t i = 0; i < root->child_count(); ++i)
                {
                    if (!code_gen_of(root->child(i)).build(o))
                    {
                        return false;
                    }
                }
                return true;
            }

            inline bool build_full(const std::string& input, WRITER& o)
            {
                statements<WRITER> p;
                statements<WRITER> s;
#ifndef DRIVER_NO_LOGIC_STATEMENTS
                code_types::register_statement(p);
                main_caller::register_statement(s);
#endif
                scope::call_hierarchy_t::calculator::register_statement(s);

                scope root_scope;
                root_scope.without_end_scope();
                p.export_to(o);
                if (!build_code(input, o))
                {
                    return false;
                }
                s.export_to(o);
                return true;
            }

            class DIALECT
            {
            public:
#ifdef DRIVER_EXTRA_MEMBERS
                DRIVER_EXTRA_MEMBERS
#endif
                static const std::string& nlexer_rule()
                {
                    return DIALECT_LIB::nlexer_rule();
                }

                static const std::string& syntaxer_rule()
                {
                    return DIALECT_LIB::syntaxer_rule();
                }

                static std::shared_ptr<automata::nlp> nlp()
                {
                    return DIALECT_NLP();
                }

                static bool parse(const std::string& input, std::string& o)
                {
                    WRITER w;
                    if (!build_full(input, w))
                    {
                        return false;
                    }
                    o = w.dump();
                    return true;
                }

                class parse_wrapper
                {
                private:
                    std::shared_ptr<primitive::interrupts> functions_;

                public:
                    explicit parse_wrapper(std::shared_ptr<primitive::interrupts> functions)
                        : functions_(std::move(functions))
                    {
                        assert(functions_ != nullptr);
                    }

                    static current_file_guard with_current_file(const std::string& filename)
                    {
                        return current_file_guard(filename);
                    }

                    bool build(const std::string& input, WRITER& o)
                    {
                        return build_full(input, o);
                    }

                    bool compile(const std::string& input, primitive::simulator& sim)
                    {
                        WRITER o;
                        if (!build(input, o))
                        {
                            return false;
                        }
                        std::string dumped = o.dump();
#ifdef COMPILE_DUMPER
                        COMPILE_DUMPER(dumped, functions_, sim);
#else
                        if (functions_)
                        {
                            sim.mem().intr() = *functions_;
                        }
                        logic::importer imp(functions_);
                        return imp.import(dumped, sim);
#endif
                    }

                    bool compile(const std::string& input, std::shared_ptr<primitive::simulator>& sim)
                    {
                        sim = std::make_shared<primitive::simulator>();
                        return compile(input, *sim);
                    }

                    bool compile_file(const std::string& filename, primitive::simulator& sim)
                    {
                        std::ifstream ifs(filename);
                        if (!ifs.is_open())
                        {
                            raise_error("Cannot read content from ", filename);
                            return false;
                        }
                        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                        current_file_guard guard(filename);
                        return compile(content, sim);
                    }
                };

                using compile_wrapper = parse_wrapper;

                static parse_wrapper with_functions(std::shared_ptr<primitive::interrupts> functions)
                {
                    return parse_wrapper(std::move(functions));
                }

                static parse_wrapper with_functions(primitive::interrupts functions)
                {
                    return parse_wrapper(std::make_shared<primitive::interrupts>(std::move(functions)));
                }

                static parse_wrapper with_default_functions()
                {
                    return with_functions(std::make_shared<primitive::interrupts>());
                }
            };
        }
    }
}

#undef DIALECT_NLP
#undef DIALECT_LIB
#undef DIALECT_NS
#undef DIALECT_CONCAT
#undef DIALECT_CONCAT_INNER
#undef DIALECT
#undef WRITER
#ifdef DRIVER_NO_LOGIC_STATEMENTS
#undef DRIVER_NO_LOGIC_STATEMENTS
#endif
#ifdef DRIVER_EXTRA_MEMBERS
#undef DRIVER_EXTRA_MEMBERS
#endif
#ifdef COMPILE_DUMPER
#undef COMPILE_DUMPER
#endif
