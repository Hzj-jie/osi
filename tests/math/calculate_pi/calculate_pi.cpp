#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include "../../../service/math/big_udec.hpp"

using namespace osi::math;

int main(int argc, char* argv[]) {
    uint32_t max_iterations = 500;
    std::string export_fraction_file;

    if (argc >= 2) {
        max_iterations = static_cast<uint32_t>(std::stoul(argv[1]));
    }
    if (argc >= 3) {
        export_fraction_file = argv[2];
    }

    std::cout << "Calculating pi (Newton arctangent series) up to " << max_iterations << " iterations..." << std::endl;

    big_udec sum(big_uint(2U), big_uint(1U));
    big_udec term(big_uint(2U), big_uint(3U));

    for (uint32_t i = 2; i <= max_iterations; ++i) {
        sum = sum + term;
        big_udec factor(big_uint(i), big_uint(2 * i + 1));
        term = term * factor;

        if (i % 100 == 0) {
            sum.reduce_fraction();
            term.reduce_fraction();
            std::cout << "@ step " << i << " -> pi = " << sum.str(50) << std::endl;
        }
    }

    sum.reduce_fraction();

    if (!export_fraction_file.empty()) {
        std::ofstream ofs(export_fraction_file);
        if (ofs.is_open()) {
            ofs << sum.fractional_str();
            std::cout << "Exported exact fraction to " << export_fraction_file << std::endl;
        }
    } else {
        std::cout << "\nFinal pi (" << max_iterations << " steps):\n" << sum.fractional_str() << std::endl;
    }

    return 0;
}
