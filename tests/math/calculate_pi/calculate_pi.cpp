#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include "../../../service/math/big_udec.hpp"

using namespace osi::math;

bool save_checkpoint(const std::string& path, uint64_t step, const big_udec& sum) {
    std::ofstream ofs(path);
    if (!ofs.is_open()) return false;
    ofs << step << "\n";
    ofs << sum.fractional_str() << "\n";
    return true;
}

bool load_checkpoint(const std::string& path, uint64_t& step, big_udec& sum, big_udec& term) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return false;
    std::string line_step, line_sum;
    if (!std::getline(ifs, line_step) || !std::getline(ifs, line_sum)) return false;
    step = static_cast<uint64_t>(std::stoull(line_step));
    if (!big_udec::parse_fraction(line_sum, sum)) return false;

    term = big_udec(big_uint(2U), big_uint(3U));
    for (uint64_t k = 2; k <= step; ++k) {
        big_udec factor(big_uint(k), big_uint(2 * k + 1));
        term.multiply(factor);
        if (k % 1000 == 0) {
            term.reduce_fraction();
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    uint64_t max_iterations = 500;
    std::string checkpoint_file;
    std::string resume_file;

    if (argc >= 2) {
        max_iterations = static_cast<uint64_t>(std::stoull(argv[1]));
    }
    if (argc >= 3) {
        checkpoint_file = argv[2];
    }
    if (argc >= 4) {
        resume_file = argv[3];
    }

    uint64_t step = 1;
    big_udec sum{big_uint(2U), big_uint(1U)};
    big_udec term{big_uint(2U), big_uint(3U)};
    uint64_t start_step = 2;

    if (!resume_file.empty()) {
        if (load_checkpoint(resume_file, step, sum, term)) {
            start_step = step + 1;
            std::cout << "Resumed pi calculation from checkpoint step " << step << std::endl;
        } else {
            std::cerr << "Error: Failed to load resume checkpoint from " << resume_file << std::endl;
            return 1;
        }
    } else if (!checkpoint_file.empty()) {
        if (load_checkpoint(checkpoint_file, step, sum, term)) {
            start_step = step + 1;
            std::cout << "Auto-resuming pi calculation from existing checkpoint step " << step << std::endl;
        }
    }

    std::cout << "Calculating pi (Newton arctangent series) from step " << start_step << " up to " << max_iterations << " iterations..." << std::endl;

    for (uint64_t i = start_step; i <= max_iterations; ++i) {
        sum.add(term);
        big_udec factor(big_uint(i), big_uint(2 * i + 1));
        term.multiply(factor);

        if (i % 1000 == 0 || i == max_iterations) {
            sum.reduce_fraction();
            term.reduce_fraction();
            std::cout << "@ step " << i << " -> pi = " << sum.str(50) << " [" << sum.fractional_str() << "]" << std::endl;

            if (!checkpoint_file.empty()) {
                save_checkpoint(checkpoint_file, i, sum);
            }
        }
    }

    sum.reduce_fraction();
    std::cout << "\nFinal pi (" << max_iterations << " steps):\n" << sum.fractional_str() << std::endl;
    return 0;
}
