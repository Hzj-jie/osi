
#pragma once
#include "../../envs/envs.hpp"
#include "../../envs/exeinfo.hpp"
#include "../../envs/processor.hpp"
#include "../../envs/deploys.hpp"
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

        // exeinfo assertions
        if (!utt_assert.is_false(exeinfo.path().empty())) return false;
        if (!utt_assert.is_false(exeinfo.name().empty())) return false;
        if (!utt_assert.is_false(exeinfo.directory().empty())) return false;
        if (!utt_assert.more(exeinfo.id(), int64_t(0))) return false;

        // processor assertions
        if (!utt_assert.more(processor.count, uint32_t(0))) return false;
        if (!utt_assert.equal(processor.single, processor.count == 1)) return false;

        // deploys assertions
        if (!utt_assert.is_false(deploys.service_name().empty())) return false;
        if (!utt_assert.is_false(deploys.deploys_folder().empty())) return false;
        if (!utt_assert.is_false(deploys.apps_folder().empty())) return false;
        if (!utt_assert.is_false(deploys.data_folder().empty())) return false;
        if (!utt_assert.is_false(deploys.log_folder().empty())) return false;
        if (!utt_assert.is_false(deploys.temp_folder().empty())) return false;
        if (!utt_assert.is_false(deploys.service_data_folder().empty())) return false;
        if (!utt_assert.is_false(deploys.service_log_folder().empty())) return false;
        if (!utt_assert.is_false(deploys.service_temp_folder().empty())) return false;
        if (!utt_assert.is_false(deploys.application_info_output_filename().empty())) return false;

        return true;
    }

    DEFINE_CASE(envs_test);
};

REGISTER_CASE(envs_test);

