#pragma once
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cstddef>
#include "data_block.hpp"
#include "data_ref.hpp"
#include "interrupts.hpp"
#include "../../app_info/assert.hpp"

namespace primitive
{
    struct call_state
    {
        uint64_t return_ip;
        size_t stack_size;
    };

    class memory_space
    {
    private:
        std::vector<data_block> stack_;
        std::vector<call_state> call_stack_;
        std::unordered_map<uint64_t, std::vector<data_block>> heap_;
        uint64_t next_heap_id_{0};
        interrupts intr_;

    public:
        memory_space() = default;

        interrupts& intr() { return intr_; }
        const interrupts& intr() const { return intr_; }

        // Stack Operations
        std::vector<data_block>& stack() { return stack_; }
        const std::vector<data_block>& stack() const { return stack_; }
        size_t stack_size() const { return stack_.size(); }

        void push_stack(data_block b)
        {
            stack_.push_back(std::move(b));
        }

        bool pop_stack(data_block& out)
        {
            if (stack_.empty()) return false;
            out = std::move(stack_.back());
            stack_.pop_back();
            return true;
        }

        bool get_stack_ref(const data_ref& ref, data_block*& out)
        {
            if (ref.type == ref_type::abs)
            {
                if (ref.offset < 0 || static_cast<size_t>(ref.offset) >= stack_.size())
                    return false;
                out = &stack_[static_cast<size_t>(ref.offset)];
                return true;
            }
            else if (ref.type == ref_type::rel)
            {
                if (stack_.empty()) return false;
                int64_t idx = static_cast<int64_t>(stack_.size()) - 1 - ref.offset;
                if (idx < 0 || static_cast<size_t>(idx) >= stack_.size())
                    return false;
                out = &stack_[static_cast<size_t>(idx)];
                return true;
            }
            return false;
        }

        // Heap Operations
        uint64_t alloc_heap(size_t slot_count)
        {
            uint64_t id = next_heap_id_++;
            heap_[id].resize(slot_count);
            return id << 32;
        }

        bool free_heap(uint64_t raw_ptr)
        {
            uint32_t heap_id = static_cast<uint32_t>(raw_ptr >> 32);
            auto it = heap_.find(heap_id);
            if (it == heap_.end()) return false;
            heap_.erase(it);
            return true;
        }

        std::vector<data_block>* get_heap_array(uint32_t heap_id)
        {
            auto it = heap_.find(heap_id);
            if (it == heap_.end()) return nullptr;
            return &it->second;
        }

        // Resolve data_ref to target data_block pointer (Stack or Heap)
        bool resolve_ref(const data_ref& ref, data_block*& out)
        {
            if (ref.type == ref_type::abs || ref.type == ref_type::rel)
            {
                return get_stack_ref(ref, out);
            }
            else
            {
                // Heap reference (habs or hrel): first resolve stack ref containing heap pointer
                data_ref stack_ref = (ref.type == ref_type::habs) ? data_ref::abs(ref.offset) : data_ref::rel(ref.offset);
                data_block* heap_id_block = nullptr;
                if (!get_stack_ref(stack_ref, heap_id_block) || heap_id_block == nullptr)
                    return false;

                uint64_t raw_ptr = static_cast<uint64_t>(heap_id_block->as_int64());
                uint32_t heap_id = static_cast<uint32_t>(raw_ptr >> 32);
                uint32_t slot_idx = static_cast<uint32_t>(raw_ptr & 0xFFFFFFFF);

                auto* heap_arr = get_heap_array(heap_id);
                if (heap_arr == nullptr || slot_idx >= heap_arr->size())
                    return false;
                out = &((*heap_arr)[slot_idx]);
                return true;
            }
        }

        // Call Stack Operations
        std::vector<call_state>& call_stack() { return call_stack_; }
        const std::vector<call_state>& call_stack() const { return call_stack_; }

        void push_call_state(uint64_t return_ip, size_t stack_size)
        {
            call_stack_.push_back({return_ip, stack_size});
        }

        bool pop_call_state(call_state& out)
        {
            if (call_stack_.empty()) return false;
            out = call_stack_.back();
            call_stack_.pop_back();
            return true;
        }

        void clear()
        {
            stack_.clear();
            call_stack_.clear();
            heap_.clear();
            next_heap_id_ = 0;
        }
    };
}
