
#pragma once
#include <iostream>
#include <string>
#include <boost/current_function.hpp>
#include "../utils/strutils.hpp"
#include "../const/character.hpp"
#include "../utils/lazier.hpp"
#include <sstream>

struct code_position;
static std::ostream& operator<<(std::ostream&, const code_position&);

struct code_position
{
private:
    const lazier<std::string> formated_str;

    static void output(std::ostream& os,
                       const char* const file,
                       const unsigned int line,
                       const char* const func)
    {
        os << character.blank
           << character.at
           << character.blank
           << func
           << character.blank
           << character.at
           << character.blank
           << file
           << character.colon
           << line;
    }

    static bool valid(const char* const file,
                      const unsigned int line,
                      const char* const func)
    {
        return file != nullptr ||
               line > 0 ||
               func != nullptr;
    }

public:
    const char* const file;
    const unsigned int line;
    const char* const func;

    code_position(const char* const file = nullptr,
                  const unsigned int line = 0,
                  const char* const func = nullptr) :
        formated_str([=]()
                     {
                         std::ostringstream os;
                         output(os, file, line, func);
                         return os.str();
                     }),
        file(file),
        line(line),
        func(func) { }

    operator bool() const
    {
        return valid(file, line, func);
    }

    const std::string& str() const
    {
        return formated_str.value();
    }

    operator std::string() const
    {
        return str();
    }

    friend std::ostream& operator<<(std::ostream& os, const code_position& cp)
    {
        code_position::output(os, cp.file, cp.line, cp.func);
        return os;
    }
};

#define CODE_POSITION() code_position(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION)
#define CODE_POS CODE_POSITION()

