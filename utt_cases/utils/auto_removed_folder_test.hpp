#include "../../utt/icase.hpp"
#include "../../utils/auto_removed_folder.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../envs/deploys.hpp"
#include "../../utils/uuid.hpp"
#include <filesystem>

class auto_removed_folder_test : public icase
{
public:
    bool run() override
    {
        using namespace std::filesystem;
        path p(deploys.service_temp_folder());
        p /= uuid_str();
        if(utt_assert.is_false(exists(p)))
        {
            {
                auto_removed_folder f(p);
                utt_assert.is_true(exists(p));
            }
            utt_assert.is_false(exists(p));
        }
        return true;
    }
    DEFINE_CASE(auto_removed_folder_test);
};

REGISTER_CASE(auto_removed_folder_test);
