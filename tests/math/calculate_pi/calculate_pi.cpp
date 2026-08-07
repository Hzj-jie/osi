#include <iostream>
#include <string>
#include <cstdint>
#include "../../../service/math/big_udec.hpp"

using namespace osi::math;

int main(int argc, char* argv[]) {
    uint32_t max_iterations = 500;
    if (argc >= 2) {
        max_iterations = static_cast<uint32_t>(std::stoul(argv[1]));
    }

    std::cout << "Calculating pi (Newton arctangent series) up to " << max_iterations << " iterations..." << std::endl;

    big_udec sum(big_uint(2U), big_uint(1U));
    big_udec term(big_uint(2U), big_uint(3U));

    for (uint32_t i = 2; i <= max_iterations; ++i) {
        sum = sum + term;
        big_udec factor(big_uint(i), big_uint(2 * i + 1));
        term = term * factor;

        if (i % 10 == 0 || i == max_iterations) {
            std::cout << "@ step " << i << " -> " << sum.str(50) << " (fraction: " << sum.fractional_str() << ")" << std::endl;
        }
    }

    std::cout << "Final pi (" << max_iterations << " steps): " << sum.str(100) << std::endl;
    return 0;
}
