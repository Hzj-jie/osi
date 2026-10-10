#ifndef DIALECT
#error "DIALECT must be defined before including namespaces.inc"
#endif
#ifndef WRITER
#error "WRITER must be defined before including namespaces.inc"
#endif

namespace osi
{
    namespace compiler
    {
        namespace DIALECT
        {
            class namespace_node : public code_gen<WRITER>
            {
            public:
                static const char* node_name() { return "namespace"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, WRITER& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() >= 4);
                    assert(scope::current()->features().with_namespace());
                    std::string ns_name = n->child(1)->input_without_ignored();
                    auto guard = scope::current()->current_namespace().define(
                        current_namespace_t::of(ns_name));
                    for (uint32_t i = 3; i + 1 < n->child_count(); ++i)
                    {
                        if (!code_gen_of(n->child(i)).build(o))
                        {
                            return false;
                        }
                    }
                    return true;
                }
            };
        }
    }
}

#undef DIALECT
#undef WRITER
