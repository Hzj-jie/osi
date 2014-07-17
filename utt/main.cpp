
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
#include "../envs/processor.hpp"
#include "utt_error_handle.hpp"
#include <atomic>
#include <chrono>
using namespace utt;
using namespace std;

static atomic<uint32_t> runned_cases;

static void run(int id)
{
    static atomic_int using_processors;
    while(1)
    {
        icase* i = nullptr;
        if(fetch_next_case(i))
        {
            ::assert(i != nullptr, code_position());
            icase& c = (*i);
            if(config.selected(c))
            {
                uint32_t current_using_processors = using_processors.fetch_add(c.preserved_processor_count());
                bool should_run = (current_using_processors <= processor.count ||
                                   current_using_processors == c.preserved_processor_count());
                if(should_run)
                {
                    int64_t start_ms = nowadays.high_res.milliseconds();
                    ::raise_error(strcat(id, ": starts case ", c.name()));
                    runned_cases++;
                    utt_assert.is_true(c.run(), CODE_POSITION());
                    ::raise_error(strcat(id,
                                         ": finished case ",
                                         c.name(),
                                         ", uses ",
                                         nowadays.high_res.milliseconds() - start_ms,
                                         " milliseconds"));
                }
                using_processors.fetch_sub(c.preserved_processor_count());
                if(should_run) finish_case(i);
                else
                {
                    pending_case(i);
                    this_thread::sleep_for(chrono::seconds(10));
                }
            }
        }
        else
        {
            ::raise_error(strcat(id, ": no more cases to run, finished"));
            break;
        }
    }
}

int main()
{
    error_handle::enable_default_file_error_writer();
    vector<thread> threads;
    for(uint32_t i = 0; i < config.thread_count - 1; i++)
        threads.push_back(thread(run, i));
    run(config.thread_count - 1);
    for(uint32_t i = 0; i < config.thread_count - 1; i++)
        threads[i].join();
    utt_raise_error(strcat("finish running all ",
                           runned_cases,
                           " cases, total failures ",
                           utt_assert.failure_count()));
    return 0;
}

