#include <iostream>
#include <string>
#include <cstdint>
#include "../../../service/math/big_udec.hpp"

using namespace osi::math;

int main(int argc, char* argv[]) {
    uint32_t max_iterations = 500;
    size_t precision = 150;
    if (argc >= 2) {
        max_iterations = static_cast<uint32_t>(std::stoul(argv[1]));
    }
    if (argc >= 3) {
        precision = static_cast<size_t>(std::stoul(argv[2]));
    }

    std::cout << "Calculating pi (Newton arctangent series) up to " << max_iterations << " iterations (precision: " << precision << " digits)..." << std::endl;

    big_udec sum(big_uint(2U), big_uint(1U));
    big_udec term(big_uint(2U), big_uint(3U));

    for (uint32_t i = 2; i <= max_iterations; ++i) {
        sum = sum + term;
        big_udec factor(big_uint(i), big_uint(2 * i + 1));
        term = term * factor;

        if (i % 100 == 0) {
            sum.reduce_fraction();
            term.reduce_fraction();
            std::cout << "@ step " << i << " -> pi = " << sum.str(precision) << std::endl;
        }
    }

    sum.reduce_fraction();
    std::cout << "\nFinal pi (" << max_iterations << " steps, " << precision << " decimal places):\n" << sum.str(precision) << std::endl;
    return 0;
}
