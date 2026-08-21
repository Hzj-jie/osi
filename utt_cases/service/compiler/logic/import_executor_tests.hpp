#pragma once
#include "import_executor_case.hpp"
#include "import_executor_cases.hpp"
#include "../../../../service/math/big_uint.hpp"
#include <cstring>

namespace logic_test
{
    namespace
    {
        inline bool parse_chunks(const std::vector<uint8_t>& b, std::vector<std::vector<uint8_t>>& out)
        {
            constexpr uint32_t chunk_checksum_mask = 0x00480048;
            out.clear();
            size_t offset = 0;
            while (offset < b.size())
            {
                if (offset + 8 > b.size()) return false;
                uint32_t len = 0, chk = 0;
                std::memcpy(&len, b.data() + offset, 4);
                std::memcpy(&chk, b.data() + offset + 4, 4);
                if (chk != (len ^ chunk_checksum_mask)) return false;
                offset += 8;
                if (offset + len > b.size()) return false;
                out.emplace_back(b.begin() + offset, b.begin() + offset + len);
                offset += len;
            }
            return true;
        }

        inline std::vector<uint8_t> str_bytes(const std::string& s)
        {
            return std::vector<uint8_t>(s.begin(), s.end());
        }

        inline std::vector<uint8_t> uint32_bytes(uint32_t val)
        {
            std::vector<uint8_t> res(4);
            std::memcpy(res.data(), &val, 4);
            return res;
        }

        inline std::vector<uint8_t> bool_bytes(bool val)
        {
            return {static_cast<uint8_t>(val ? 1 : 0)};
        }
    }

    class import_executor_test1 : public import_executor_case
    {
    public:
        import_executor_test1()
            : import_executor_case(import_cases::case1) {}

    protected:
        void check_result(primitive::simulator& e) override
        {
            utt_assert.equal(e.stack_size(), static_cast<size_t>(8));
            utt_assert.equal(e.access(primitive::data_ref::abs(0)), str_bytes("hello world\n"));
            utt_assert.equal(e.access(primitive::data_ref::abs(1)), str_bytes("\ndlrow olleh"));
            utt_assert.equal(e.access(primitive::data_ref::abs(2)), str_bytes("\ndlrow olleh"));
            utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(3)), static_cast<uint32_t>(0));
            utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(4)), static_cast<uint32_t>(1));
            utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(5)), static_cast<uint32_t>(12));
            utt_assert.equal(e.access_as_uint32(primitive::data_ref::abs(6)), static_cast<uint32_t>(0));
            utt_assert.equal(e.access_as_bool(primitive::data_ref::abs(7)), false);
        }

        DEFINE_CASE(import_executor_test1);
    };

    class import_executor_test2 : public import_executor_case
    {
    public:
        import_executor_test2()
            : import_executor_case(import_cases::case2) {}

    protected:
        void check_result(primitive::simulator& e) override
        {
            auto b = e.access(primitive::data_ref::abs(0));
            std::vector<std::vector<uint8_t>> chunks;
            if (utt_assert.is_true(parse_chunks(b, chunks)) &&
                utt_assert.equal(chunks.size(), static_cast<size_t>(100)))
            {
                for (size_t i = 0; i < chunks.size(); i++)
                {
                    osi::math::big_uint fact = osi::math::big_uint::factorial(static_cast<uint32_t>(i + 1));
                    utt_assert.equal(chunks[i], fact.as_bytes());
                }
            }
        }

        DEFINE_CASE(import_executor_test2);
    };

    class import_executor_test3 : public import_executor_case
    {
    public:
        import_executor_test3()
            : import_executor_case(import_cases::case3) {}

    protected:
        void check_result(primitive::simulator& e) override
        {
            auto b = e.access(primitive::data_ref::abs(0));
            std::vector<std::vector<uint8_t>> chunks;
            if (utt_assert.is_true(parse_chunks(b, chunks)) &&
                utt_assert.equal(chunks.size(), static_cast<size_t>(100)))
            {
                for (size_t i = 0; i < chunks.size(); i++)
                {
                    utt_assert.equal(chunks[i], uint32_bytes(static_cast<uint32_t>(i + 1)));
                }
            }
        }

        DEFINE_CASE(import_executor_test3);
    };

    class import_executor_test4 : public import_executor_case
    {
    private:
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;

    public:
        import_executor_test4()
            : import_executor_test4(std::make_shared<primitive::console_io::test_wrapper>()) {}

        explicit import_executor_test4(std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper)
            : import_executor_case(import_cases::case4, std::make_shared<primitive::interrupts>(io_wrapper->io())),
              io_wrapper_(io_wrapper) {}

    protected:
        void check_result(primitive::simulator&) override
        {
            utt_assert.equal(io_wrapper_->output(), std::string("hello world"));
            utt_assert.equal(io_wrapper_->error(), std::string("hello world"));
        }

        DEFINE_CASE(import_executor_test4);
    };

    class import_executor_heap : public import_executor_case
    {
    private:
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;

    public:
        import_executor_heap()
            : import_executor_heap(std::make_shared<primitive::console_io::test_wrapper>()) {}

        explicit import_executor_heap(std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper)
            : import_executor_case(import_cases::heap, std::make_shared<primitive::interrupts>(io_wrapper->io())),
              io_wrapper_(io_wrapper) {}

    protected:
        void check_result(primitive::simulator&) override
        {
            std::string expected(1, static_cast<char>(99));
            utt_assert.equal(io_wrapper_->output(), expected);
        }

        DEFINE_CASE(import_executor_heap);
    };

    class import_executor_callee_ref : public import_executor_case
    {
    private:
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;

    public:
        import_executor_callee_ref()
            : import_executor_callee_ref(std::make_shared<primitive::console_io::test_wrapper>()) {}

        explicit import_executor_callee_ref(std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper)
            : import_executor_case(import_cases::callee_ref, std::make_shared<primitive::interrupts>(io_wrapper->io())),
              io_wrapper_(io_wrapper) {}

    protected:
        void check_result(primitive::simulator&) override
        {
            utt_assert.equal(io_wrapper_->output(), std::string(4, 'd') + " ");
        }

        DEFINE_CASE(import_executor_callee_ref);
    };

    class import_executor_callee_ref2 : public import_executor_case
    {
    private:
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;

    public:
        import_executor_callee_ref2()
            : import_executor_callee_ref2(std::make_shared<primitive::console_io::test_wrapper>()) {}

        explicit import_executor_callee_ref2(std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper)
            : import_executor_case(import_cases::callee_ref2, std::make_shared<primitive::interrupts>(io_wrapper->io())),
              io_wrapper_(io_wrapper) {}

    protected:
        void check_result(primitive::simulator&) override
        {
            utt_assert.equal(io_wrapper_->output().length(), static_cast<size_t>(4));
            if (io_wrapper_->output().length() == 4)
            {
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[0]), 1);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[1]), 2);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[2]), 3);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[3]), 4);
            }
        }

        DEFINE_CASE(import_executor_callee_ref2);
    };

    class import_executor_address_of : public import_executor_case
    {
    private:
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;

    public:
        import_executor_address_of()
            : import_executor_address_of(std::make_shared<primitive::console_io::test_wrapper>()) {}

        explicit import_executor_address_of(std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper)
            : import_executor_case(import_cases::address_of, std::make_shared<primitive::interrupts>(io_wrapper->io())),
              io_wrapper_(io_wrapper) {}

    protected:
        void check_result(primitive::simulator&) override
        {
            utt_assert.equal(io_wrapper_->output().length(), static_cast<size_t>(8));
            if (io_wrapper_->output().length() == 8)
            {
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[0]), 2);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[1]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[2]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[3]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[4]), 1);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[5]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[6]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[7]), 0);
            }
        }

        DEFINE_CASE(import_executor_address_of);
    };

    class import_executor_address_of_ref : public import_executor_case
    {
    private:
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;

    public:
        import_executor_address_of_ref()
            : import_executor_address_of_ref(std::make_shared<primitive::console_io::test_wrapper>()) {}

        explicit import_executor_address_of_ref(std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper)
            : import_executor_case(import_cases::address_of_ref, std::make_shared<primitive::interrupts>(io_wrapper->io())),
              io_wrapper_(io_wrapper) {}

    protected:
        void check_result(primitive::simulator&) override
        {
            utt_assert.equal(io_wrapper_->output().length(), static_cast<size_t>(8));
            if (io_wrapper_->output().length() == 8)
            {
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[0]), 2);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[1]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[2]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[3]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[4]), 1);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[5]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[6]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[7]), 0);
            }
        }

        DEFINE_CASE(import_executor_address_of_ref);
    };

    class import_executor_address_of_param : public import_executor_case
    {
    private:
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;

    public:
        import_executor_address_of_param()
            : import_executor_address_of_param(std::make_shared<primitive::console_io::test_wrapper>()) {}

        explicit import_executor_address_of_param(std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper)
            : import_executor_case(import_cases::address_of_param, std::make_shared<primitive::interrupts>(io_wrapper->io())),
              io_wrapper_(io_wrapper) {}

    protected:
        void check_result(primitive::simulator&) override
        {
            utt_assert.equal(io_wrapper_->output().length(), static_cast<size_t>(8));
            if (io_wrapper_->output().length() == 8)
            {
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[0]), 2);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[1]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[2]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[3]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[4]), 1);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[5]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[6]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[7]), 0);
            }
        }

        DEFINE_CASE(import_executor_address_of_param);
    };

    class import_executor_address_of_address_of_param : public import_executor_case
    {
    private:
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;

    public:
        import_executor_address_of_address_of_param()
            : import_executor_address_of_address_of_param(std::make_shared<primitive::console_io::test_wrapper>()) {}

        explicit import_executor_address_of_address_of_param(std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper)
            : import_executor_case(import_cases::address_of_address_of_param, std::make_shared<primitive::interrupts>(io_wrapper->io())),
              io_wrapper_(io_wrapper) {}

    protected:
        void check_result(primitive::simulator&) override
        {
            utt_assert.equal(io_wrapper_->output().length(), static_cast<size_t>(4));
            if (io_wrapper_->output().length() == 4)
            {
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[0]), 1);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[1]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[2]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[3]), 0);
            }
        }

        DEFINE_CASE(import_executor_address_of_address_of_param);
    };

    class import_executor_copy_address_of : public import_executor_case
    {
    private:
        std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper_;

    public:
        import_executor_copy_address_of()
            : import_executor_copy_address_of(std::make_shared<primitive::console_io::test_wrapper>()) {}

        explicit import_executor_copy_address_of(std::shared_ptr<primitive::console_io::test_wrapper> io_wrapper)
            : import_executor_case(import_cases::copy_address_of, std::make_shared<primitive::interrupts>(io_wrapper->io())),
              io_wrapper_(io_wrapper) {}

    protected:
        void check_result(primitive::simulator&) override
        {
            utt_assert.equal(io_wrapper_->output().length(), static_cast<size_t>(4));
            if (io_wrapper_->output().length() == 4)
            {
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[0]), 2);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[1]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[2]), 0);
                utt_assert.equal(static_cast<int>(io_wrapper_->output()[3]), 0);
            }
        }

        DEFINE_CASE(import_executor_copy_address_of);
    };

    REGISTER_CASE(import_executor_test1);
    REGISTER_CASE(import_executor_test2);
    REGISTER_CASE(import_executor_test3);
    REGISTER_CASE(import_executor_test4);
    REGISTER_CASE(import_executor_heap);
    REGISTER_CASE(import_executor_callee_ref);
    REGISTER_CASE(import_executor_callee_ref2);
    REGISTER_CASE(import_executor_address_of);
    REGISTER_CASE(import_executor_address_of_ref);
    REGISTER_CASE(import_executor_address_of_param);
    REGISTER_CASE(import_executor_address_of_address_of_param);
    REGISTER_CASE(import_executor_copy_address_of);
}
