#include <iostream>
#include <string>
#include <cstdint>
#include "../../../service/math/big_udec.hpp"

using namespace osi::math;

int main(int argc, char* argv[]) {
    uint32_t max_iterations = 1000;
    if (argc >= 2) {
        max_iterations = static_cast<uint32_t>(std::stoul(argv[1]));
    }

    std::cout << "Calculating e up to " << max_iterations << " iterations..." << std::endl;

    big_udec sum(1U);
    big_udec cur(1U);

    for (uint32_t i = 1; i <= max_iterations; ++i) {
        cur = cur / big_udec(i);
        sum = sum + cur;

        if (i % 10 == 0 || i == max_iterations) {
            std::cout << "@ step " << i << " -> " << sum.str(50) << " (fraction: " << sum.fractional_str() << ")" << std::endl;
        }
    }

    std::cout << "Final e (" << max_iterations << " steps): " << sum.str(100) << std::endl;
    return 0;
}
