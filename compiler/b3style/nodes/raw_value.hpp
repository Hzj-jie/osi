#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../../../app_info/assert.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../code_gen/includes.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "../../logic/scope.hpp"
#include "../../../interpreter/primitive/data_block.hpp"
#include "../../../math/big_int.hpp"
#include "../../../math/big_dec.hpp"
#include "../scope.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class raw_value : public code_gen<logic_writer>
            {
            private:
                std::string code_type;

            public:
                explicit raw_value(std::string type)
                    : code_type(std::move(type))
                {
                    assert(!code_type.empty());
                }

                static bool build(const primitive::data_block& data, const std::string& code_type, logic_writer& o)
                {
                    auto target = scope::current()->value_target().with_temp_target(code_type, o).front();
                    return builders::of_copy_const(target, data).to(o);
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    primitive::data_block data;
                    if (!parse(n, data))
                    {
                        return false;
                    }
                    return build(data, code_type, o);
                }

            protected:
                virtual bool parse(const std::shared_ptr<automata::typed_node>&, primitive::data_block&) = 0;
            };

            class bool_node : public raw_value
            {
            public:
                inline static const std::string type_name = "::Boolean";

                static const char* node_name() { return "bool"; }

                bool_node() : raw_value(type_name) {}

                using raw_value::build;
                static bool build(bool i, logic_writer& o)
                {
                    return raw_value::build(primitive::data_block(i), type_name, o);
                }

            protected:
                bool parse(const std::shared_ptr<automata::typed_node>& n, primitive::data_block& o) override
                {
                    std::string s = n->word().str();
                    bool val = (s == "true" || s == "True" || s == "TRUE");
                    o = primitive::data_block(val);
                    return true;
                }
            };

            class integer_node : public raw_value
            {
            public:
                inline static const std::string type_name = "::Integer";

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
                    std::string s = n->word().str();
                    try
                    {
                        int32_t val = std::stol(s);
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
                inline static const std::string type_name = "::BigUnsignedInteger";

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
                inline static const std::string type_name = "::BigUnsignedFloat";

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
                inline static const std::string type_name = "::String";

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
        }
    }
}
