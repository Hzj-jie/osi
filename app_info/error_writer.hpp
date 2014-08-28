
#pragma once
#include <stdint.h>
#include <utility>
#include <memory>
#include <string>
#include <iostream>
#include <initializer_list>
#include "k_assert.hpp"
#include <boost/filesystem/fstream.hpp>
#include <array>
#include "../sync/lock.hpp"
#include <mutex>

namespace error_handle
{
    class ierror_writer
    {
    public:
        virtual void write(const std::string& s) = 0;
        virtual ~ierror_writer() = default;
    };

    class console_error_writer : public ierror_writer
    {
    public:
        void write(const std::string& s) override
        {
            static_scope_lock();
            std::cout << s;
        }
    };

    class file_error_writer : public ierror_writer
    {
    private:
        // std::ofstream is not moveable in g++ 4.8.2
        boost::filesystem::ofstream* const writer;
        std::mutex mtx;

    public:
        void write(const std::string& s) override
        {
            k_assert(writer != nullptr);
            if(*writer)
            {
                scope_lock(mtx);
                ((*writer) << s).flush();
            }
        }

        template <typename T>
        file_error_writer(T&& file) :
            writer(new boost::filesystem::ofstream(file)) { }

        file_error_writer(file_error_writer&& other) :
            writer(std::move(other.writer)) { }

        ~file_error_writer() override
        {
            k_assert(writer != nullptr);
            writer->close();
            delete writer;
        }
    };

    template <typename IMPL_T>
    class error_type_selected_error_writer : public ierror_writer
    {
    private:
        IMPL_T impl;
        std::array<bool, uint32_t(error_type::last) - uint32_t(error_type::first) - 1> selected;
    public:
#define ERROR_TYPE_SELECTED_ERROR_WRITER_CTOR \
    impl(std::forward<IMPL_T>(impl)) { \
        selected.fill(false); \
        for(auto it = input_selected.begin(); it != input_selected.end(); it++) { \
            if(uint32_t(*it) > uint32_t(error_type::first) && \
               uint32_t(*it) < uint32_t(error_type::last)) \
                selected[uint32_t(*it) - uint32_t(error_type::first) - 1] = true; } }
        error_type_selected_error_writer(const IMPL_T& impl,
                                         const std::initializer_list<error_type>& input_selected) :
            ERROR_TYPE_SELECTED_ERROR_WRITER_CTOR;

        error_type_selected_error_writer(IMPL_T&& impl,
                                         const std::initializer_list<error_type>& input_selected) :
            ERROR_TYPE_SELECTED_ERROR_WRITER_CTOR;
#undef ERROR_TYPE_SELECTED_ERROR_WRITER_CTOR

        void write(const std::string& s) override
        {
            if(!s.empty())
            {
                if(selected[uint32_t(char_to_error_type(s[0])) - uint32_t(error_type::first) - 1])
                    impl.write(s);
            }
        }
    };
}

