#pragma once
#include "../../../interpreter/primitive/simulator.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace primitive_test
{
    class executor_arithmetic_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            simulator sim;
            // push 10, push 20, add (stack[0] = stack[1] + stack[2])
            std::vector<instruction> insts = {
                instruction(command_type::push), // idx 0
                instruction(command_type::push), // idx 1
                instruction(command_type::push), // idx 2
                instruction(command_type::cpc, data_ref::abs(1), data_block::from_int64(10)),
                instruction(command_type::cpc, data_ref::abs(2), data_block::from_int64(20)),
                instruction(command_type::add, {data_ref::abs(0), data_ref::abs(1), data_ref::abs(2)}),
                instruction(command_type::stop)
            };

            sim.load_instructions(insts);
            utt_assert.is_true(sim.run());

            data_block* res = nullptr;
            utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(0), res));
            utt_assert.equal(res->as_int64(), 30LL);

            return true;
        }

        DEFINE_CASE(executor_arithmetic_test);
    };

    class executor_jump_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            simulator sim;
            // 0: push, 1: cpc abs(0)=1, 2: cpc abs(0)=2 (jumped over), 3: stop
            std::vector<instruction> insts = {
                instruction(command_type::push),
                instruction(command_type::cpc, data_ref::abs(0), data_block::from_int64(1)),
                instruction(command_type::jump, {data_ref::abs(4)}),
                instruction(command_type::cpc, data_ref::abs(0), data_block::from_int64(999)),
                instruction(command_type::stop)
            };

            sim.load_instructions(insts);
            utt_assert.is_true(sim.run());

            data_block* res = nullptr;
            utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(0), res));
            utt_assert.equal(res->as_int64(), 1LL);

            return true;
        }

        DEFINE_CASE(executor_jump_test);
    };

    class executor_string_slice_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            simulator sim;
            // 0: push dst, 1: push src, 2: push offset, 3: push len
            std::vector<instruction> insts = {
                instruction(command_type::push),
                instruction(command_type::push),
                instruction(command_type::push),
                instruction(command_type::push),
                instruction(command_type::cpc, data_ref::abs(1), data_block::from_string("hello_world")),
                instruction(command_type::cpc, data_ref::abs(2), data_block::from_int64(6)),
                instruction(command_type::cpc, data_ref::abs(3), data_block::from_int64(5)),
                instruction(command_type::cutl, {data_ref::abs(0), data_ref::abs(1), data_ref::abs(2), data_ref::abs(3)}),
                instruction(command_type::stop)
            };

            sim.load_instructions(insts);
            utt_assert.is_true(sim.run());

            data_block* res = nullptr;
            utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(0), res));
            utt_assert.equal(res->as_string(), std::string("world"));

            return true;
        }

        DEFINE_CASE(executor_string_slice_test);
    };

    class executor_big_uint_arithmetic_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            using namespace osi::math;

            // 1. Test arbitrary-precision multiplication & division in primitive
            {
                simulator sim;
                // stack[0] = prod, stack[1] = rem, stack[2] = a, stack[3] = b, stack[4] = quot
                big_uint big_a("123456789012345678901234567890");
                big_uint big_b("987654321098765432109876543210");

                std::vector<instruction> insts = {
                    instruction(command_type::push), // 0: prod
                    instruction(command_type::push), // 1: rem
                    instruction(command_type::push), // 2: a
                    instruction(command_type::push), // 3: b
                    instruction(command_type::push), // 4: quot
                    instruction(command_type::cpc, data_ref::abs(2), data_block::from_big_uint(big_a)),
                    instruction(command_type::cpc, data_ref::abs(3), data_block::from_big_uint(big_b)),
                    instruction(command_type::mul, {data_ref::abs(0), data_ref::abs(2), data_ref::abs(3)}),
                    instruction(command_type::div, {data_ref::abs(4), data_ref::abs(1), data_ref::abs(0), data_ref::abs(2)}),
                    instruction(command_type::stop)
                };

                sim.load_instructions(insts);
                utt_assert.is_true(sim.run());

                data_block* prod_block = nullptr;
                data_block* quot_block = nullptr;
                data_block* rem_block = nullptr;
                utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(0), prod_block));
                utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(4), quot_block));
                utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(1), rem_block));

                utt_assert.equal(prod_block->as_big_uint().str(), (big_a * big_b).str());
                utt_assert.equal(quot_block->as_big_uint().str(), big_b.str());
                utt_assert.is_true(rem_block->as_big_uint().is_zero());
            }

            // 2. Test carry over and divided by zero flags
            {
                simulator sim;
                // stack[0] = res, stack[1] = b1(5), stack[2] = b2(10), stack[3] = co_flag, stack[4] = dbz_flag
                std::vector<instruction> insts = {
                    instruction(command_type::push), // 0
                    instruction(command_type::push), // 1
                    instruction(command_type::push), // 2
                    instruction(command_type::push), // 3
                    instruction(command_type::push), // 4
                    instruction(command_type::cpc, data_ref::abs(1), data_block::from_int64(5)),
                    instruction(command_type::cpc, data_ref::abs(2), data_block::from_int64(10)),
                    instruction(command_type::sub, {data_ref::abs(0), data_ref::abs(1), data_ref::abs(2)}),
                    instruction(command_type::cpco, data_ref::abs(3)),
                    instruction(command_type::cpc, data_ref::abs(2), data_block::from_int64(0)),
                    instruction(command_type::div, {data_ref::abs(0), data_ref::abs(1), data_ref::abs(1), data_ref::abs(2)}),
                    instruction(command_type::cpdbz, data_ref::abs(4)),
                    instruction(command_type::stop)
                };

                sim.load_instructions(insts);
                utt_assert.is_true(sim.run());

                data_block* co_block = nullptr;
                data_block* dbz_block = nullptr;
                utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(3), co_block));
                utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(4), dbz_block));

                utt_assert.is_true(co_block->as_bool());
                utt_assert.is_true(dbz_block->as_bool());
            }

            return true;
        }

        DEFINE_CASE(executor_big_uint_arithmetic_test);
    };

    class executor_big_udec_arithmetic_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            using namespace osi::math;

            // 1. Test fractional calculations: 1/3 + 1/6 = 1/2
            {
                simulator sim;
                big_udec one_third(big_uint(1ULL), big_uint(3ULL));
                big_udec one_sixth(big_uint(1ULL), big_uint(6ULL));

                std::vector<instruction> insts = {
                    instruction(command_type::push), // 0: sum
                    instruction(command_type::push), // 1: 1/3
                    instruction(command_type::push), // 2: 1/6
                    instruction(command_type::cpc, data_ref::abs(1), data_block::from_big_udec(one_third)),
                    instruction(command_type::cpc, data_ref::abs(2), data_block::from_big_udec(one_sixth)),
                    instruction(command_type::fadd, {data_ref::abs(0), data_ref::abs(1), data_ref::abs(2)}),
                    instruction(command_type::stop)
                };

                sim.load_instructions(insts);
                utt_assert.is_true(sim.run());

                data_block* sum_block = nullptr;
                utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(0), sum_block));

                big_udec sum = sum_block->as_big_udec();
                sum.reduce_fraction();
                utt_assert.equal(sum.numerator().as_uint64(), 1ULL);
                utt_assert.equal(sum.denominator().as_uint64(), 2ULL);
            }

            // 2. Test imaginary number flag on fsub underflow
            {
                simulator sim;
                big_udec half(big_uint(1ULL), big_uint(2ULL));
                big_udec one(big_uint(1ULL), big_uint(1ULL));

                std::vector<instruction> insts = {
                    instruction(command_type::push), // 0: diff
                    instruction(command_type::push), // 1: 1/2
                    instruction(command_type::push), // 2: 1
                    instruction(command_type::push), // 3: in_flag
                    instruction(command_type::cpc, data_ref::abs(1), data_block::from_big_udec(half)),
                    instruction(command_type::cpc, data_ref::abs(2), data_block::from_big_udec(one)),
                    instruction(command_type::fsub, {data_ref::abs(0), data_ref::abs(1), data_ref::abs(2)}),
                    instruction(command_type::cpin, data_ref::abs(3)),
                    instruction(command_type::stop)
                };

                sim.load_instructions(insts);
                utt_assert.is_true(sim.run());

                data_block* in_block = nullptr;
                utt_assert.is_true(sim.mem().get_stack_ref(data_ref::abs(3), in_block));
                utt_assert.is_true(in_block->as_bool());
            }

            return true;
        }

        DEFINE_CASE(executor_big_udec_arithmetic_test);
    };

    REGISTER_CASE(executor_arithmetic_test);
    REGISTER_CASE(executor_jump_test);
    REGISTER_CASE(executor_string_slice_test);
    REGISTER_CASE(executor_big_uint_arithmetic_test);
    REGISTER_CASE(executor_big_udec_arithmetic_test);
}
