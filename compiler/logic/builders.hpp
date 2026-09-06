#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "../function_signature.hpp"
#include "logic_writer.hpp"
#include "builders.implementation.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            namespace builders
            {
                using parameter_type = osi::compiler::parameter_type;
                using parameter = osi::compiler::parameter;

                struct start_scope_wrapper
                {
                    logic_writer& o;

                    explicit start_scope_wrapper(logic_writer& writer) : o(writer) {}

                    bool of(const std::function<bool(logic_writer&)>& f)
                    {
                        return of_start_scope(f).to(o);
                    }

                    bool of(const std::function<bool()>& f)
                    {
                        assert(f != nullptr);
                        return of_start_scope([f](logic_writer&) { return f(); }).to(o);
                    }
                };

                inline start_scope_wrapper start_scope(logic_writer& o)
                {
                    return start_scope_wrapper(o);
                }

                inline callee_builder_16 of_callee(const std::string& s_1,
                                                   const std::string& s_2,
                                                   const std::vector<parameter>& params_3,
                                                   const std::function<bool(logic_writer&)>& para_4)
                {
                    std::vector<std::pair<std::string, std::string>> tp;
                    tp.reserve(params_3.size());
                    for (const auto& p : params_3)
                    {
                        tp.emplace_back(p.name, p.full_type());
                    }
                    return callee_builder_16(s_1, s_2, std::move(tp), para_4);
                }

                inline callee_ref_builder_19 of_callee_ref(const std::string& s_1,
                                                           const std::string& s_2,
                                                           const std::vector<parameter_type>& params_3)
                {
                    std::vector<std::string> ps;
                    ps.reserve(params_3.size());
                    for (const auto& p : params_3)
                    {
                        ps.push_back(p.full_type());
                    }
                    return callee_ref_builder_19(s_1, s_2, std::move(ps));
                }

                inline callee_ref_builder_19 of_callee_ref(const std::string& s_1,
                                                           const std::string& s_2,
                                                           const std::vector<parameter>& params_3)
                {
                    std::vector<std::string> ps;
                    ps.reserve(params_3.size());
                    for (const auto& p : params_3)
                    {
                        ps.push_back(p.full_type());
                    }
                    return callee_ref_builder_19(s_1, s_2, std::move(ps));
                }

                inline while_then_builder_47 of_while_then(const std::string& cond, const std::function<bool()>& f)
                {
                    assert(f != nullptr);
                    return of_while_then(cond, [f](logic_writer&) { return f(); });
                }

                inline if_builder_29 of_if(const std::string& cond, const std::function<bool()>& f)
                {
                    assert(f != nullptr);
                    return of_if(cond, [f](logic_writer&) { return f(); });
                }

                inline if_builder_28 of_if(const std::string& cond, const std::function<bool()>& f1, const std::function<bool()>& f2)
                {
                    assert(f1 != nullptr && f2 != nullptr);
                    return of_if(cond, [f1](logic_writer&) { return f1(); }, [f2](logic_writer&) { return f2(); });
                }
            }
        }

        namespace builders = logic::builders;
    }
}
