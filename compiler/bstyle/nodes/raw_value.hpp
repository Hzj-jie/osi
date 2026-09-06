#pragma once
#include <string>
#include <memory>
#include <vector>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../../interpreter/primitive/data_block.hpp"
#include "../../../math/big_uint.hpp"
#include "../../../math/big_udec.hpp"
#include "../../../utils/strutils.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../code_gen/includes.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "../scope.hpp"

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            class raw_value : public code_gen<logic_writer>
            {
            protected:
                std::string code_type_;

                explicit raw_value(std::string code_type)
                    : code_type_(std::move(code_type))
                {
                    assert(!code_type_.empty());
                }

                virtual bool parse(const std::shared_ptr<automata::typed_node>& n, primitive::data_block& o) = 0;

                static bool build(const primitive::data_block& i,
                                  const std::string& code_type,
                                  logic_writer& o)
                {
                    assert(!code_type.empty());
                    auto target = scope::current()->value_target().with_temp_target(code_type, o).front();
                    return builders::of_copy_const(target, i).to(o);
                }

            public:
                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->leaf());
                    primitive::data_block d;
                    if (!parse(n, d))
                    {
                        raise_error("Cannot parse data to ", code_type_, "\n", n->trace_back_str());
                        return false;
                    }
                    return build(d, code_type_, o);
                }
            };

            class bool_node : public raw_value
            {
            public:
                inline static const std::string type_name = "Boolean";

                static const char* node_name() { return "bool"; }

                bool_node() : raw_value(type_name) {}

            protected:
                bool parse(const std::shared_ptr<automata::typed_node>& n, primitive::data_block& o) override
                {
                    std::string s = n->word().str();
                    if (s == "true")
                    {
                        o = primitive::data_block(true);
                        return true;
                    }
                    if (s == "false")
                    {
                        o = primitive::data_block(false);
                        return true;
                    }
                    return false;
                }
            };

            class integer_node : public raw_value
            {
            public:
                inline static const std::string type_name = "Integer";

                static const char* node_name() { return "integer"; }

                integer_node() : raw_value(type_name) {}

                using raw_value::build;
                static bool build(int32_t i, logic_writer& o)
                {
                    return raw_value::build(primitive::data_block(i), type_name, o);
                }

            protected:
                bool parse(const std::shared_ptr<automata::typed_node>& n, primitive::data_block& o) override
                {
                    try
                    {
                        int32_t val = std::stol(n->word().str());
                        o = primitive::data_block(val);
                        return true;
                    }
                    catch (...)
                    {
                        return false;
                    }
                }
            };

            class biguint_node : public raw_value
            {
            public:
                inline static const std::string type_name = "BigUnsignedInteger";

                static const char* node_name() { return "biguint"; }

                biguint_node() : raw_value(type_name) {}

            protected:
                bool parse(const std::shared_ptr<automata::typed_node>& n, primitive::data_block& o) override
                {
                    std::string s = n->word().str();
                    if (!s.empty() && (s.back() == 'l' || s.back() == 'L'))
                    {
                        s.pop_back();
                    }
                    math::big_uint i(s);
                    o = primitive::data_block(i.as_bytes());
                    return true;
                }
            };

            class ufloat_node : public raw_value
            {
            public:
                inline static const std::string type_name = "BigUnsignedFloat";

                static const char* node_name() { return "ufloat"; }

                ufloat_node() : raw_value(type_name) {}

            protected:
                bool parse(const std::shared_ptr<automata::typed_node>& n, primitive::data_block& o) override
                {
                    std::string s = n->word().str();
                    size_t dot_pos = s.find('.');
                    if (dot_pos == std::string::npos)
                    {
                        math::big_udec u(math::big_uint(s), math::big_uint(1ULL));
                        o = primitive::data_block(u.as_bytes());
                        return true;
                    }
                    std::string int_part = s.substr(0, dot_pos);
                    std::string frac_part = s.substr(dot_pos + 1);
                    if (int_part.empty()) int_part = "0";
                    math::big_uint num(int_part);
                    math::big_uint den(1ULL);
                    for (size_t k = 0; k < frac_part.size(); ++k)
                    {
                        den.multiply(10ULL);
                        num.multiply(10ULL);
                    }
                    if (!frac_part.empty())
                    {
                        num.add(math::big_uint(frac_part));
                    }
                    math::big_udec u(num, den);
                    u.reduce_fraction();
                    o = primitive::data_block(u.as_bytes());
                    return true;
                }
            };

            class string_node : public raw_value
            {
            public:
                inline static const std::string type_name = "String";

                static const char* node_name() { return "string"; }

                string_node() : raw_value(type_name) {}

                using raw_value::build;
                static bool build(const std::string& i, logic_writer& o)
                {
                    return raw_value::build(primitive::data_block(i), type_name, o);
                }

            protected:
                bool parse(const std::shared_ptr<automata::typed_node>& n, primitive::data_block& o) override
                {
                    std::string s = n->word().str();
                    s = trim(s, "\"");
                    s = c_unescape(s);
                    o = primitive::data_block(s);
                    return true;
                }
            };

            class logic_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "logic"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 2);
                    std::string s = n->child(1)->word().str();
                    s = trim(s, "\"");
                    s = c_unescape(s);
                    o.append(s);
                    o.append("\n");
                    return true;
                }
            };
        }
    }
}
