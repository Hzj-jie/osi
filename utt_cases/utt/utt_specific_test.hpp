
#pragma once
#include "utt_failure_test.hpp"
#include "../../utt/case_wrapper/specific_case_wrapper.hpp"

class utt_specific_test :
        public specific_case_wrapper<utt_failure_test>
{ };
REGISTER_CASE(utt_specific_test);

