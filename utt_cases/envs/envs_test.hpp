
#pragma once
#include "../../envs/envs.hpp"
#include "../../utt/icase.hpp"
#include "../../utils/strcmp.hpp"
#include "../../const/character.hpp"
#include <map>
#include <string>

class envs_test : public icase
{
private:
    void valid_str(const std::string& s)
    {
        utt_assert.is_false(str_contains(s, character.null), s, " has null");
    }
public:
    bool run() override
    {
        using namespace std;
        const map<string, string>& envs = ::envs();
        for(auto it : envs)
        {
            valid_str(it.first);
            valid_str(it.second);
        }
        for(auto it : envs)
        {
            utt_assert.equal(::envs[it.first], it.second);
            utt_assert.equal(::envs[it.first.c_str()], it.second);
            {
                string s;
                utt_assert.is_true(::envs(it.first, s), it.first, " not found");
                utt_assert.equal(s, it.second);
                utt_assert.is_true(::envs(it.first.c_str(), s), it.first, ".c_str() not found");
                utt_assert.equal(s, it.second);
            }
        }
        return true;
    }

    DEFINE_CASE(envs_test);
};

REGISTER_CASE(envs_test);

