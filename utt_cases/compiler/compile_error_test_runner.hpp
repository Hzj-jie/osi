#pragma once

#include <iostream>
#include <string>
#include <initializer_list>
#include "../../app_info/error_handle.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

namespace osi::compiler
{
    class compile_error_test_runner : public icase
    {
    protected:
        virtual bool parse_error_case(const std::string& content)
        {
            (void)content;
            return false;
        }

        void run_error(const std::string& content, std::initializer_list<std::string> errs)
        {
            std::string log = error_handle::capture_log([&]() {
                utt_assert.is_false(parse_error_case(content));
            });
            for (const auto& e : errs)
            {
                if (log.find(e) == std::string::npos)
                {
                    std::cerr << "Expected error substring [" << e << "] not found in captured log:\n"
                              << log << std::endl;
                }
                utt_assert.is_true(log.find(e) != std::string::npos);
            }
        }
    };
}
