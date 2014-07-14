
#pragma once
#include "../../utils/uuid.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include <string>

class uuid_test : public icase
{
public:
    virtual bool run()
    {
        using namespace std;
        utt_assert.equal(uuid_short_str().size(), 32);
        utt_assert.equal(uuid_long_str().size(), 36);
        utt_assert.equal(uuid_short_str().find('-'), string::npos);
        utt_assert.not_equal(uuid_long_str().find('-'), string::npos);
        utt_assert.not_equal(uuid_short_str(), uuid_short_str());
        utt_assert.not_equal(uuid_long_str(), uuid_long_str());
        utt_assert.not_equal(uuid_str(), uuid_str());
        utt_assert.not_equal(uuid_original_str(), uuid_original_str());
        return true;
    }
    DEFINE_CASE(uuid_test);
};

REGISTER_CASE(uuid_test);

