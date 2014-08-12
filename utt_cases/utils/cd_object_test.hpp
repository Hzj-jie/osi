
#pragma once
#include "../../utils/cd_object.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../utils/random.hpp"
#include <stdint.h>
#include <string>

class cd_object_test : public icase
{
private:
    template <typename T>
    bool run_case(T&& value = T())
    {
        const uint32_t count = 100;
        for(uint32_t i = 0; i < count; i++)
        {
            cd_object<T> o(value);
            utt_assert.equal(cd_object<T>::create_times(), i + 1);
            utt_assert.equal(cd_object<T>::destruct_times(), i);
            utt_assert.equal(cd_object<T>::instance_count(), 1);
            utt_assert.equal(value, *o);
        }
        utt_assert.equal(cd_object<T>::create_times(), count);
        utt_assert.equal(cd_object<T>::destruct_times(), count);
        utt_assert.equal(cd_object<T>::instance_count(), 0);

        cd_object<T>::reset();
        return true;
    }

public:
    bool execute() override
    {
        return run_case<std::string>("abc") &&
               run_case<int>(rnd_int()) &&
               run_case<uint32_t>(rnd_uint());
    }

    DEFINE_CASE(cd_object_test);
};
REGISTER_CASE(cd_object_test);

