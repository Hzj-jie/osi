
#pragma once
#include <iostream>
#include <string>
#include <boost/current_function.hpp>
#include "../utils/strutils.hpp"
#include "../const/character.hpp"
#include "../utils/lazier.hpp"

struct code_position
{
private:
    static bool valid(const char* const file,
                      const unsigned int line,
                      const char* const func)
    {
        return file != nullptr ||
               line > 0 ||
               func != nullptr;
    }

    static std::string format(const char* const file,
                              const unsigned int line,
                              const char* const func)
    {
        if(valid(file, line, func))
        {
#define format_character(x) static_cast<const char>(character.x)
            // the compiler cannot deduce the type, since character.at is a const intergeral
            return strcat(format_character(at),
                          format_character(blank),
                          func,
                          format_character(blank),
                          format_character(at),
                          format_character(blank),
                          file,
                          format_character(colon),
                          line);
#undef format_character
        }
        else return std::string();
    }

private:
    mutable lazier<std::string> formated_str;

public:
    const char* const file;
    const unsigned int line;
    const char* const func;

    code_position(const char* const file = nullptr,
                  const unsigned int line = 0,
                  const char* const func = nullptr) :
        formated_str([=]() { return format(file, line, func); }),
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
};

#define CODE_POSITION() code_position(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION)
#define CODE_POS CODE_POSITION()
#define EMPTY_CODE_POSITION() static_cast<const code_position*>(nullptr)
#define EMPTY_CODE_POS EMPTY_CODE_POSITION()

static std::ostream& operator <<(std::ostream& os, const code_position& cp)
{
    os << cp.str();
    return os;
}

