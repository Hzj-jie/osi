// This file is generated from definition.txt. Do not edit.
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <utility>
#include "../../app_info/assert.hpp"
#include "../../interpreter/primitive/data_block.hpp"
#include "logic_writer.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            namespace builders
            {
                struct start_scope_builder_1
                {
                    std::function<bool(logic_writer&)> paragraph_1;

                    start_scope_builder_1(const std::function<bool(logic_writer&)>& p_paragraph_1)
                        : paragraph_1(p_paragraph_1)
                    {
                        assert(paragraph_1 != nullptr);
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("start_scope") &&
                               o.append("{") &&
                               o.append(paragraph_1) &&
                               o.append("}") &&
                               o.append("\n");
                    }
                };

                inline start_scope_builder_1 of_start_scope(const std::function<bool(logic_writer&)>& paragraph_1)
                {
                    return start_scope_builder_1(paragraph_1);
                }

                struct type_builder_2
                {
                    std::string string_1;
                    uint32_t uint_2;

                    type_builder_2(const std::string& p_string_1, uint32_t p_uint_2)
                        : string_1(p_string_1), uint_2(p_uint_2)
                    {
                        assert(!string_1.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("type") &&
                               o.append(string_1) &&
                               o.append(uint_2) &&
                               o.append("\n");
                    }
                };

                inline type_builder_2 of_type(const std::string& string_1, uint32_t uint_2)
                {
                    return type_builder_2(string_1, uint_2);
                }

                struct append_slice_builder_3
                {
                    std::string string_1;
                    std::string string_2;

                    append_slice_builder_3(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("append_slice") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline append_slice_builder_3 of_append_slice(const std::string& string_1, const std::string& string_2)
                {
                    return append_slice_builder_3(string_1, string_2);
                }

                struct cut_slice_builder_4
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    cut_slice_builder_4(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("cut_slice") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline cut_slice_builder_4 of_cut_slice(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return cut_slice_builder_4(string_1, string_2, string_3);
                }

                struct cut_builder_5
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    cut_builder_5(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("cut") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline cut_builder_5 of_cut(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return cut_builder_5(string_1, string_2, string_3);
                }

                struct cut_len_builder_6
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;
                    std::string string_4;

                    cut_len_builder_6(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3, const std::string& p_string_4)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3), string_4(p_string_4)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                        assert(!string_4.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("cut_len") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append(string_4) &&
                               o.append("\n");
                    }
                };

                inline cut_len_builder_6 of_cut_len(const std::string& string_1, const std::string& string_2, const std::string& string_3, const std::string& string_4)
                {
                    return cut_len_builder_6(string_1, string_2, string_3, string_4);
                }

                struct clear_builder_7
                {
                    std::string string_1;

                    clear_builder_7(const std::string& p_string_1)
                        : string_1(p_string_1)
                    {
                        assert(!string_1.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("clear") &&
                               o.append(string_1) &&
                               o.append("\n");
                    }
                };

                inline clear_builder_7 of_clear(const std::string& string_1)
                {
                    return clear_builder_7(string_1);
                }

                struct add_builder_8
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    add_builder_8(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("add") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline add_builder_8 of_add(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return add_builder_8(string_1, string_2, string_3);
                }

                struct subtract_builder_9
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    subtract_builder_9(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("subtract") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline subtract_builder_9 of_subtract(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return subtract_builder_9(string_1, string_2, string_3);
                }

                struct multiply_builder_10
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    multiply_builder_10(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("multiply") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline multiply_builder_10 of_multiply(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return multiply_builder_10(string_1, string_2, string_3);
                }

                struct divide_builder_11
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;
                    std::string string_4;

                    divide_builder_11(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3, const std::string& p_string_4)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3), string_4(p_string_4)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                        assert(!string_4.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("divide") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append(string_4) &&
                               o.append("\n");
                    }
                };

                inline divide_builder_11 of_divide(const std::string& string_1, const std::string& string_2, const std::string& string_3, const std::string& string_4)
                {
                    return divide_builder_11(string_1, string_2, string_3, string_4);
                }

                struct extract_builder_12
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;
                    std::string string_4;

                    extract_builder_12(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3, const std::string& p_string_4)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3), string_4(p_string_4)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                        assert(!string_4.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("extract") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append(string_4) &&
                               o.append("\n");
                    }
                };

                inline extract_builder_12 of_extract(const std::string& string_1, const std::string& string_2, const std::string& string_3, const std::string& string_4)
                {
                    return extract_builder_12(string_1, string_2, string_3, string_4);
                }

                struct power_builder_13
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    power_builder_13(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("power") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline power_builder_13 of_power(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return power_builder_13(string_1, string_2, string_3);
                }

                struct and_builder_14
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    and_builder_14(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("and") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline and_builder_14 of_and(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return and_builder_14(string_1, string_2, string_3);
                }

                struct or_builder_15
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    or_builder_15(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("or") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline or_builder_15 of_or(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return or_builder_15(string_1, string_2, string_3);
                }

                struct callee_builder_16
                {
                    std::string string_1;
                    std::string string_2;
                    std::vector<std::pair<std::string, std::string>> typed_parameters_3;
                    std::function<bool(logic_writer&)> paragraph_4;

                    callee_builder_16(const std::string& p_string_1, const std::string& p_string_2, const std::vector<std::pair<std::string, std::string>>& p_typed_parameters_3, const std::function<bool(logic_writer&)>& p_paragraph_4)
                        : string_1(p_string_1), string_2(p_string_2), typed_parameters_3(p_typed_parameters_3), paragraph_4(p_paragraph_4)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(paragraph_4 != nullptr);
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("callee") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("(") &&
                               o.append(typed_parameters_3) &&
                               o.append(")") &&
                               o.append("{") &&
                               o.append(paragraph_4) &&
                               o.append("}") &&
                               o.append("\n");
                    }
                };

                inline callee_builder_16 of_callee(const std::string& string_1, const std::string& string_2, const std::vector<std::pair<std::string, std::string>>& typed_parameters_3, const std::function<bool(logic_writer&)>& paragraph_4)
                {
                    return callee_builder_16(string_1, string_2, typed_parameters_3, paragraph_4);
                }

                struct caller_builder_17
                {
                    std::string string_1;
                    std::string string_2;
                    std::vector<std::string> parameters_3;

                    caller_builder_17(const std::string& p_string_1, const std::string& p_string_2, const std::vector<std::string>& p_parameters_3)
                        : string_1(p_string_1), string_2(p_string_2), parameters_3(p_parameters_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("caller") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("(") &&
                               o.append(parameters_3) &&
                               o.append(")") &&
                               o.append("\n");
                    }
                };

                inline caller_builder_17 of_caller(const std::string& string_1, const std::string& string_2, const std::vector<std::string>& parameters_3)
                {
                    return caller_builder_17(string_1, string_2, parameters_3);
                }

                struct caller_builder_18
                {
                    std::string string_1;
                    std::vector<std::string> parameters_2;

                    caller_builder_18(const std::string& p_string_1, const std::vector<std::string>& p_parameters_2)
                        : string_1(p_string_1), parameters_2(p_parameters_2)
                    {
                        assert(!string_1.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("caller") &&
                               o.append(string_1) &&
                               o.append("(") &&
                               o.append(parameters_2) &&
                               o.append(")") &&
                               o.append("\n");
                    }
                };

                inline caller_builder_18 of_caller(const std::string& string_1, const std::vector<std::string>& parameters_2)
                {
                    return caller_builder_18(string_1, parameters_2);
                }

                struct callee_ref_builder_19
                {
                    std::string string_1;
                    std::string string_2;
                    std::vector<std::string> parameters_3;

                    callee_ref_builder_19(const std::string& p_string_1, const std::string& p_string_2, const std::vector<std::string>& p_parameters_3)
                        : string_1(p_string_1), string_2(p_string_2), parameters_3(p_parameters_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("callee_ref") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("(") &&
                               o.append(parameters_3) &&
                               o.append(")") &&
                               o.append("\n");
                    }
                };

                inline callee_ref_builder_19 of_callee_ref(const std::string& string_1, const std::string& string_2, const std::vector<std::string>& parameters_3)
                {
                    return callee_ref_builder_19(string_1, string_2, parameters_3);
                }

                struct caller_ref_builder_20
                {
                    std::string string_1;
                    std::string string_2;
                    std::vector<std::string> parameters_3;

                    caller_ref_builder_20(const std::string& p_string_1, const std::string& p_string_2, const std::vector<std::string>& p_parameters_3)
                        : string_1(p_string_1), string_2(p_string_2), parameters_3(p_parameters_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("caller_ref") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("(") &&
                               o.append(parameters_3) &&
                               o.append(")") &&
                               o.append("\n");
                    }
                };

                inline caller_ref_builder_20 of_caller_ref(const std::string& string_1, const std::string& string_2, const std::vector<std::string>& parameters_3)
                {
                    return caller_ref_builder_20(string_1, string_2, parameters_3);
                }

                struct caller_ref_builder_21
                {
                    std::string string_1;
                    std::vector<std::string> parameters_2;

                    caller_ref_builder_21(const std::string& p_string_1, const std::vector<std::string>& p_parameters_2)
                        : string_1(p_string_1), parameters_2(p_parameters_2)
                    {
                        assert(!string_1.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("caller_ref") &&
                               o.append(string_1) &&
                               o.append("(") &&
                               o.append(parameters_2) &&
                               o.append(")") &&
                               o.append("\n");
                    }
                };

                inline caller_ref_builder_21 of_caller_ref(const std::string& string_1, const std::vector<std::string>& parameters_2)
                {
                    return caller_ref_builder_21(string_1, parameters_2);
                }

                struct address_of_builder_22
                {
                    std::string string_1;
                    std::string string_2;

                    address_of_builder_22(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("address_of") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline address_of_builder_22 of_address_of(const std::string& string_1, const std::string& string_2)
                {
                    return address_of_builder_22(string_1, string_2);
                }

                struct less_builder_23
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    less_builder_23(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("less") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline less_builder_23 of_less(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return less_builder_23(string_1, string_2, string_3);
                }

                struct more_builder_24
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    more_builder_24(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("more") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline more_builder_24 of_more(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return more_builder_24(string_1, string_2, string_3);
                }

                struct equal_builder_25
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    equal_builder_25(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("equal") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline equal_builder_25 of_equal(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return equal_builder_25(string_1, string_2, string_3);
                }

                struct less_or_equal_builder_26
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    less_or_equal_builder_26(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("less_or_equal") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline less_or_equal_builder_26 of_less_or_equal(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return less_or_equal_builder_26(string_1, string_2, string_3);
                }

                struct more_or_equal_builder_27
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    more_or_equal_builder_27(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("more_or_equal") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline more_or_equal_builder_27 of_more_or_equal(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return more_or_equal_builder_27(string_1, string_2, string_3);
                }

                struct if_builder_28
                {
                    std::string string_1;
                    std::function<bool(logic_writer&)> paragraph_2;
                    std::function<bool(logic_writer&)> paragraph_4;

                    if_builder_28(const std::string& p_string_1, const std::function<bool(logic_writer&)>& p_paragraph_2, const std::function<bool(logic_writer&)>& p_paragraph_4)
                        : string_1(p_string_1), paragraph_2(p_paragraph_2), paragraph_4(p_paragraph_4)
                    {
                        assert(!string_1.empty());
                        assert(paragraph_2 != nullptr);
                        assert(paragraph_4 != nullptr);
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("if") &&
                               o.append(string_1) &&
                               o.append("{") &&
                               o.append(paragraph_2) &&
                               o.append("}") &&
                               o.append("else") &&
                               o.append("{") &&
                               o.append(paragraph_4) &&
                               o.append("}") &&
                               o.append("\n");
                    }
                };

                inline if_builder_28 of_if(const std::string& string_1, const std::function<bool(logic_writer&)>& paragraph_2, const std::function<bool(logic_writer&)>& paragraph_4)
                {
                    return if_builder_28(string_1, paragraph_2, paragraph_4);
                }

                struct if_builder_29
                {
                    std::string string_1;
                    std::function<bool(logic_writer&)> paragraph_2;

                    if_builder_29(const std::string& p_string_1, const std::function<bool(logic_writer&)>& p_paragraph_2)
                        : string_1(p_string_1), paragraph_2(p_paragraph_2)
                    {
                        assert(!string_1.empty());
                        assert(paragraph_2 != nullptr);
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("if") &&
                               o.append(string_1) &&
                               o.append("{") &&
                               o.append(paragraph_2) &&
                               o.append("}") &&
                               o.append("\n");
                    }
                };

                inline if_builder_29 of_if(const std::string& string_1, const std::function<bool(logic_writer&)>& paragraph_2)
                {
                    return if_builder_29(string_1, paragraph_2);
                }

                struct copy_builder_30
                {
                    std::string string_1;
                    std::string string_2;

                    copy_builder_30(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("copy") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline copy_builder_30 of_copy(const std::string& string_1, const std::string& string_2)
                {
                    return copy_builder_30(string_1, string_2);
                }

                struct copy_const_builder_31
                {
                    std::string string_1;
                    primitive::data_block data_block_2;

                    copy_const_builder_31(const std::string& p_string_1, const primitive::data_block& p_data_block_2)
                        : string_1(p_string_1), data_block_2(p_data_block_2)
                    {
                        assert(!string_1.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("copy_const") &&
                               o.append(string_1) &&
                               o.append(data_block_2) &&
                               o.append("\n");
                    }
                };

                inline copy_const_builder_31 of_copy_const(const std::string& string_1, const primitive::data_block& data_block_2)
                {
                    return copy_const_builder_31(string_1, data_block_2);
                }

                struct define_builder_32
                {
                    std::string string_1;
                    std::string string_2;

                    define_builder_32(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("define") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline define_builder_32 of_define(const std::string& string_1, const std::string& string_2)
                {
                    return define_builder_32(string_1, string_2);
                }

                struct define_heap_builder_33
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    define_heap_builder_33(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("define_heap") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline define_heap_builder_33 of_define_heap(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return define_heap_builder_33(string_1, string_2, string_3);
                }

                struct redefine_builder_34
                {
                    std::string string_1;
                    std::string string_2;

                    redefine_builder_34(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("redefine") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline redefine_builder_34 of_redefine(const std::string& string_1, const std::string& string_2)
                {
                    return redefine_builder_34(string_1, string_2);
                }

                struct undefine_builder_35
                {
                    std::string string_1;

                    undefine_builder_35(const std::string& p_string_1)
                        : string_1(p_string_1)
                    {
                        assert(!string_1.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("undefine") &&
                               o.append(string_1) &&
                               o.append("\n");
                    }
                };

                inline undefine_builder_35 of_undefine(const std::string& string_1)
                {
                    return undefine_builder_35(string_1);
                }

                struct dealloc_heap_builder_36
                {
                    std::string string_1;

                    dealloc_heap_builder_36(const std::string& p_string_1)
                        : string_1(p_string_1)
                    {
                        assert(!string_1.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("dealloc_heap") &&
                               o.append(string_1) &&
                               o.append("\n");
                    }
                };

                inline dealloc_heap_builder_36 of_dealloc_heap(const std::string& string_1)
                {
                    return dealloc_heap_builder_36(string_1);
                }

                struct do_until_builder_37
                {
                    std::string string_1;
                    std::function<bool(logic_writer&)> paragraph_2;

                    do_until_builder_37(const std::string& p_string_1, const std::function<bool(logic_writer&)>& p_paragraph_2)
                        : string_1(p_string_1), paragraph_2(p_paragraph_2)
                    {
                        assert(!string_1.empty());
                        assert(paragraph_2 != nullptr);
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("do_until") &&
                               o.append(string_1) &&
                               o.append("{") &&
                               o.append(paragraph_2) &&
                               o.append("}") &&
                               o.append("\n");
                    }
                };

                inline do_until_builder_37 of_do_until(const std::string& string_1, const std::function<bool(logic_writer&)>& paragraph_2)
                {
                    return do_until_builder_37(string_1, paragraph_2);
                }

                struct do_while_builder_38
                {
                    std::string string_1;
                    std::function<bool(logic_writer&)> paragraph_2;

                    do_while_builder_38(const std::string& p_string_1, const std::function<bool(logic_writer&)>& p_paragraph_2)
                        : string_1(p_string_1), paragraph_2(p_paragraph_2)
                    {
                        assert(!string_1.empty());
                        assert(paragraph_2 != nullptr);
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("do_while") &&
                               o.append(string_1) &&
                               o.append("{") &&
                               o.append(paragraph_2) &&
                               o.append("}") &&
                               o.append("\n");
                    }
                };

                inline do_while_builder_38 of_do_while(const std::string& string_1, const std::function<bool(logic_writer&)>& paragraph_2)
                {
                    return do_while_builder_38(string_1, paragraph_2);
                }

                struct interrupt_builder_39
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    interrupt_builder_39(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("interrupt") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline interrupt_builder_39 of_interrupt(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return interrupt_builder_39(string_1, string_2, string_3);
                }

                struct move_builder_40
                {
                    std::string string_1;
                    std::string string_2;

                    move_builder_40(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("move") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline move_builder_40 of_move(const std::string& string_1, const std::string& string_2)
                {
                    return move_builder_40(string_1, string_2);
                }

                struct return_builder_41
                {
                    std::string string_1;

                    return_builder_41(const std::string& p_string_1)
                        : string_1(p_string_1)
                    {
                        assert(!string_1.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("return") &&
                               o.append(string_1) &&
                               o.append("*") &&
                               o.append("\n");
                    }
                };

                inline return_builder_41 of_return(const std::string& string_1)
                {
                    return return_builder_41(string_1);
                }

                struct return_builder_42
                {
                    std::string string_1;
                    std::string string_2;

                    return_builder_42(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("return") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline return_builder_42 of_return(const std::string& string_1, const std::string& string_2)
                {
                    return return_builder_42(string_1, string_2);
                }

                struct append_builder_43
                {
                    std::string string_1;
                    std::string string_2;

                    append_builder_43(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("append") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline append_builder_43 of_append(const std::string& string_1, const std::string& string_2)
                {
                    return append_builder_43(string_1, string_2);
                }

                struct not_builder_44
                {
                    std::string string_1;
                    std::string string_2;

                    not_builder_44(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("not") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline not_builder_44 of_not(const std::string& string_1, const std::string& string_2)
                {
                    return not_builder_44(string_1, string_2);
                }

                struct sizeof_builder_45
                {
                    std::string string_1;
                    std::string string_2;

                    sizeof_builder_45(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("sizeof") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline sizeof_builder_45 of_sizeof(const std::string& string_1, const std::string& string_2)
                {
                    return sizeof_builder_45(string_1, string_2);
                }

                struct empty_builder_46
                {
                    std::string string_1;
                    std::string string_2;

                    empty_builder_46(const std::string& p_string_1, const std::string& p_string_2)
                        : string_1(p_string_1), string_2(p_string_2)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("empty") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append("\n");
                    }
                };

                inline empty_builder_46 of_empty(const std::string& string_1, const std::string& string_2)
                {
                    return empty_builder_46(string_1, string_2);
                }

                struct while_then_builder_47
                {
                    std::string string_1;
                    std::function<bool(logic_writer&)> paragraph_2;

                    while_then_builder_47(const std::string& p_string_1, const std::function<bool(logic_writer&)>& p_paragraph_2)
                        : string_1(p_string_1), paragraph_2(p_paragraph_2)
                    {
                        assert(!string_1.empty());
                        assert(paragraph_2 != nullptr);
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("while_then") &&
                               o.append(string_1) &&
                               o.append("{") &&
                               o.append(paragraph_2) &&
                               o.append("}") &&
                               o.append("\n");
                    }
                };

                inline while_then_builder_47 of_while_then(const std::string& string_1, const std::function<bool(logic_writer&)>& paragraph_2)
                {
                    return while_then_builder_47(string_1, paragraph_2);
                }

                struct stop_builder_48
                {

                    stop_builder_48() = default;

                    bool to(logic_writer& o) const
                    {
                        return o.append("stop") &&
                               o.append("\n");
                    }
                };

                inline stop_builder_48 of_stop()
                {
                    return stop_builder_48();
                }

                struct float_add_builder_49
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_add_builder_49(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_add") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_add_builder_49 of_float_add(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_add_builder_49(string_1, string_2, string_3);
                }

                struct float_subtract_builder_50
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_subtract_builder_50(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_subtract") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_subtract_builder_50 of_float_subtract(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_subtract_builder_50(string_1, string_2, string_3);
                }

                struct float_multiply_builder_51
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_multiply_builder_51(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_multiply") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_multiply_builder_51 of_float_multiply(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_multiply_builder_51(string_1, string_2, string_3);
                }

                struct float_divide_builder_52
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_divide_builder_52(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_divide") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_divide_builder_52 of_float_divide(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_divide_builder_52(string_1, string_2, string_3);
                }

                struct float_extract_builder_53
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_extract_builder_53(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_extract") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_extract_builder_53 of_float_extract(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_extract_builder_53(string_1, string_2, string_3);
                }

                struct float_power_builder_54
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_power_builder_54(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_power") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_power_builder_54 of_float_power(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_power_builder_54(string_1, string_2, string_3);
                }

                struct float_less_builder_55
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_less_builder_55(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_less") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_less_builder_55 of_float_less(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_less_builder_55(string_1, string_2, string_3);
                }

                struct float_more_builder_56
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_more_builder_56(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_more") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_more_builder_56 of_float_more(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_more_builder_56(string_1, string_2, string_3);
                }

                struct float_equal_builder_57
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_equal_builder_57(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_equal") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_equal_builder_57 of_float_equal(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_equal_builder_57(string_1, string_2, string_3);
                }

                struct float_less_or_equal_builder_58
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_less_or_equal_builder_58(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_less_or_equal") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_less_or_equal_builder_58 of_float_less_or_equal(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_less_or_equal_builder_58(string_1, string_2, string_3);
                }

                struct float_more_or_equal_builder_59
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    float_more_or_equal_builder_59(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("float_more_or_equal") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline float_more_or_equal_builder_59 of_float_more_or_equal(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return float_more_or_equal_builder_59(string_1, string_2, string_3);
                }

                struct left_shift_builder_60
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    left_shift_builder_60(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("left_shift") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline left_shift_builder_60 of_left_shift(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return left_shift_builder_60(string_1, string_2, string_3);
                }

                struct right_shift_builder_61
                {
                    std::string string_1;
                    std::string string_2;
                    std::string string_3;

                    right_shift_builder_61(const std::string& p_string_1, const std::string& p_string_2, const std::string& p_string_3)
                        : string_1(p_string_1), string_2(p_string_2), string_3(p_string_3)
                    {
                        assert(!string_1.empty());
                        assert(!string_2.empty());
                        assert(!string_3.empty());
                    }

                    bool to(logic_writer& o) const
                    {
                        return o.append("right_shift") &&
                               o.append(string_1) &&
                               o.append(string_2) &&
                               o.append(string_3) &&
                               o.append("\n");
                    }
                };

                inline right_shift_builder_61 of_right_shift(const std::string& string_1, const std::string& string_2, const std::string& string_3)
                {
                    return right_shift_builder_61(string_1, string_2, string_3);
                }

            }
        }
    }
}
