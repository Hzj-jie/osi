#include <iostream>
#include <string>
#include <cstdint>
#include "../../../service/math/big_udec.hpp"

using namespace osi::math;

int main(int argc, char* argv[]) {
    uint32_t max_iterations = 200;
    size_t precision = 150;
    if (argc >= 2) {
        max_iterations = static_cast<uint32_t>(std::stoul(argv[1]));
    }
    if (argc >= 3) {
        precision = static_cast<size_t>(std::stoul(argv[2]));
    }

    std::cout << "Calculating e up to " << max_iterations << " iterations (precision: " << precision << " digits)..." << std::endl;

    big_udec sum(1U);
    big_udec cur(1U);

    for (uint32_t i = 1; i <= max_iterations; ++i) {
        cur = cur / big_udec(i);
        sum = sum + cur;

        if (i % 50 == 0) {
            sum.reduce_fraction();
            cur.reduce_fraction();
            std::cout << "@ step " << i << " -> e = " << sum.str(precision) << std::endl;
        }
    }

    sum.reduce_fraction();
    std::cout << "\nFinal e (" << max_iterations << " steps, " << precision << " decimal places):\n" << sum.str(precision) << std::endl;
    return 0;
}
