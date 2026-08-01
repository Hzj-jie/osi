#pragma once
#include "../../primitive/memory_space.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

namespace primitive_test
{
    class stack_operation_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            memory_space mem;
            mem.push_stack(data_block::from_int64(10));
            mem.push_stack(data_block::from_int64(20));
            mem.push_stack(data_block::from_int64(30));

            utt_assert.equal(mem.stack_size(), static_cast<size_t>(3));

            data_block* ptr = nullptr;
            // Abs 0 should be 10
            utt_assert.is_true(mem.get_stack_ref(data_ref::abs(0), ptr));
            utt_assert.equal(ptr->as_int64(), 10LL);

            // Rel 0 (top) should be 30
            utt_assert.is_true(mem.get_stack_ref(data_ref::rel(0), ptr));
            utt_assert.equal(ptr->as_int64(), 30LL);

            // Rel 1 (one below top) should be 20
            utt_assert.is_true(mem.get_stack_ref(data_ref::rel(1), ptr));
            utt_assert.equal(ptr->as_int64(), 20LL);

            data_block popped;
            utt_assert.is_true(mem.pop_stack(popped));
            utt_assert.equal(popped.as_int64(), 30LL);
            utt_assert.equal(mem.stack_size(), static_cast<size_t>(2));

            return true;
        }

        DEFINE_CASE(stack_operation_test);
    };

    class heap_operation_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            memory_space mem;
            uint64_t heap_id = mem.alloc_heap(5);
            utt_assert.is_true(heap_id > 0);

            auto* arr = mem.get_heap_array(heap_id);
            utt_assert.is_true(arr != nullptr);
            utt_assert.equal(arr->size(), static_cast<size_t>(5));

            (*arr)[0] = data_block::from_string("heap_value");

            // Push heap_id to stack abs 0
            mem.push_stack(data_block::from_int64(static_cast<int64_t>(heap_id)));

            data_block* resolved = nullptr;
            utt_assert.is_true(mem.resolve_ref(data_ref::habs(0), resolved));
            utt_assert.is_true(resolved != nullptr);
            utt_assert.equal(resolved->as_string(), std::string("heap_value"));

            utt_assert.is_true(mem.free_heap(heap_id));
            utt_assert.is_true(mem.get_heap_array(heap_id) == nullptr);

            return true;
        }

        DEFINE_CASE(heap_operation_test);
    };

    class call_stack_operation_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            memory_space mem;
            mem.push_call_state(105, 12);
            utt_assert.equal(mem.call_stack().size(), static_cast<size_t>(1));

            call_state cs;
            utt_assert.is_true(mem.pop_call_state(cs));
            utt_assert.equal(cs.return_ip, static_cast<uint64_t>(105));
            utt_assert.equal(cs.stack_size, static_cast<size_t>(12));
            utt_assert.is_true(mem.call_stack().empty());

            return true;
        }

        DEFINE_CASE(call_stack_operation_test);
    };

    REGISTER_CASE(stack_operation_test);
    REGISTER_CASE(heap_operation_test);
    REGISTER_CASE(call_stack_operation_test);
}
