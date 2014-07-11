
#include "icase.hpp"
#include "cases.hpp"
#include "config.hpp"
#include <stdint.h>
#include <vector>
#include <thread>
#include "../app_info/trace.hpp"
#include "../app_info/error_handle.hpp"
#include "../utils/strutils.hpp"
#include "../app_info/assert.hpp"
using namespace utt;
using namespace std;

static void run(int id)
{
    while(1)
    {
        icase* i = nullptr;
        if(fetch_next_case(i))
        {
            ::assert(i != nullptr, code_position());
            icase& c = (*i);
            if(config.selected(c))
            {
                raise_error(strcat(id, ": starts case ", c.name()));
                utt::assert.is_true(c.run(), code_position());
            }
            finish_case(i);
        }
        else raise_error(strcat(id, ": no more cases to run, finished"));
    }
}

int main()
{
    vector<thread> threads;
    for(uint32_t i = 0; i < config.thread_count - 1; i++)
        threads.push_back(thread(run, i));
    run(config.thread_count - 1);
    for(uint32_t i = 0; i < config.thread_count - 1; i++)
        threads[i].join();
    return 0;
}

