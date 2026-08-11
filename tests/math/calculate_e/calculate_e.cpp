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

bool load_checkpoint(const std::string& path, uint64_t& step, big_udec& sum, big_uint& fact) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return false;
    std::string line_step, line_sum;
    if (!std::getline(ifs, line_step) || !std::getline(ifs, line_sum)) return false;
    step = static_cast<uint64_t>(std::stoull(line_step));
    if (!big_udec::parse_fraction(line_sum, sum)) return false;

    fact.set_one();
    for (uint64_t k = 1; k <= step; ++k) {
        if (k <= 0xFFFFFFFFULL) {
            fact.multiply(static_cast<uint32_t>(k));
        } else {
            fact = fact * big_uint(k);
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    uint64_t max_iterations = 200;
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

    uint64_t step = 0;
    big_udec sum{1U};
    big_uint fact{1U};
    uint64_t start_step = 1;

    if (!resume_file.empty()) {
        if (load_checkpoint(resume_file, step, sum, fact)) {
            start_step = step + 1;
            std::cout << "Resumed e calculation from checkpoint step " << step << std::endl;
        } else {
            std::cerr << "Error: Failed to load resume checkpoint from " << resume_file << std::endl;
            return 1;
        }
    } else if (!checkpoint_file.empty()) {
        if (load_checkpoint(checkpoint_file, step, sum, fact)) {
            start_step = step + 1;
            std::cout << "Auto-resuming e calculation from existing checkpoint step " << step << std::endl;
        }
    }

    big_udec c{1U};
    for (uint64_t k = 1; k < start_step; ++k) {
        c.divide(big_udec(k));
    }

    std::cout << "Calculating e from step " << start_step << " up to " << max_iterations << " iterations..." << std::endl;

    for (uint64_t i = start_step; i <= max_iterations; ++i) {
        c.divide(big_udec(i));
        sum.add(c);

        if (i % 1000 == 0 || i == max_iterations) {
            sum.reduce_fraction();
            std::cout << "@ step " << i << " -> e = " << sum.str(50) << " [" << sum.fractional_str() << "]" << std::endl;

            if (!checkpoint_file.empty()) {
                save_checkpoint(checkpoint_file, i, sum);
            }
        }
    }

    sum.reduce_fraction();
    std::cout << "\nFinal e (" << max_iterations << " steps):\n" << sum.fractional_str() << std::endl;
    return 0;
}
