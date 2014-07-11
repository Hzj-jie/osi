
#include <stdint.h>
#include <utility>
#include <memory>
#include <string>
#include <iostream>
#include <initializer_list>
#include "k_assert.hpp"
#include <boost/filesystem/fstream.hpp>
#include <array>

namespace error_handle
{
    class ierror_writer
    {
    public:
        virtual void write(const std::string& s) = 0;
        virtual ~ierror_writer() { }
    };

    class console_error_writer : public ierror_writer
    {
    public:
        virtual void write(const std::string& s)
        {
            std::cout << s;
        }
    };

    class file_error_writer : public ierror_writer
    {
    private:
        // std::ofstream is not moveable in g++ 4.8.2
        boost::filesystem::ofstream* const writer;
    public:
        typedef boost::filesystem::path::string_type path_string;
        virtual void write(const std::string& s)
        {
            k_assert(writer != nullptr);
            if(*writer) (*writer) << s;
        }

        file_error_writer(const path_string& file) :
            writer(new boost::filesystem::ofstream(file)) { }

        file_error_writer(file_error_writer&& other) :
            writer(std::move(other.writer)) { }

        virtual ~file_error_writer()
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
    impl(impl) { \
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

        virtual void write(const std::string& s)
        {
            if(!s.empty())
            {
                if(selected[uint32_t(char_to_error_type(s[0])) - uint32_t(error_type::first) - 1])
                    impl.write(s);
            }
        }
    };
}

