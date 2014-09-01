
#pragma once
#include "../../utils/outputter.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include <string>

class outputter_test : public icase
{
private:
    class not_visible { };

public:
    bool run() override
    {
        utt_assert.is_true(outputter<int>::visible());
        utt_assert.is_true(outputter<std::string>::visible());
        utt_assert.is_false(outputter<not_visible>::visible());

        utt_assert.equal(outputter<int>(100).to_str(), std::string("100"));
        utt_assert.equal(outputter<std::string>(std::string("abc")).to_str(), std::string("abc"));
        utt_assert.equal(outputter<not_visible>(not_visible()).to_str(), std::string("[not_visible]"));

        return true;
    }

    DEFINE_CASE(outputter_test);
};
REGISTER_CASE(outputter_test);

