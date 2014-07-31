
#pragma once

// MSVC includes assert in atomic
#include <atomic>

#include <boost/lockfree/queue.hpp>

#undef assert

