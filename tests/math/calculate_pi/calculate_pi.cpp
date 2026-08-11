#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include "../../../service/math/big_udec.hpp"

using namespace osi::math;

struct checkpoint_pi {
    uint64_t step = 1;
    big_udec sum{big_uint(2U), big_uint(1U)};
    big_udec term{big_uint(2U), big_uint(3U)};
};

bool save_checkpoint(const std::string& path, uint64_t step, const big_udec& sum, const big_udec& term) {
    std::ofstream ofs(path);
    if (!ofs.is_open()) return false;
    ofs << step << "\n";
    ofs << sum.fractional_str() << "\n";
    ofs << term.fractional_str() << "\n";
    return true;
}

bool load_checkpoint(const std::string& path, checkpoint_pi& chk) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return false;
    std::string line_step, line_sum, line_term;
    if (!std::getline(ifs, line_step) || !std::getline(ifs, line_sum) || !std::getline(ifs, line_term)) return false;
    chk.step = static_cast<uint64_t>(std::stoull(line_step));
    if (!big_udec::parse_fraction(line_sum, chk.sum)) return false;
    if (!big_udec::parse_fraction(line_term, chk.term)) return false;
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

    checkpoint_pi chk;
    uint64_t start_step = 2;

    if (!resume_file.empty()) {
        if (load_checkpoint(resume_file, chk)) {
            start_step = chk.step + 1;
            std::cout << "Resumed pi calculation from checkpoint step " << chk.step << std::endl;
        } else {
            std::cerr << "Error: Failed to load resume checkpoint from " << resume_file << std::endl;
            return 1;
        }
    } else if (!checkpoint_file.empty()) {
        if (load_checkpoint(checkpoint_file, chk)) {
            start_step = chk.step + 1;
            std::cout << "Auto-resuming pi calculation from existing checkpoint step " << chk.step << std::endl;
        }
    }

    big_udec sum = chk.sum;
    big_udec term = chk.term;

    std::cout << "Calculating pi (Newton arctangent series) from step " << start_step << " up to " << max_iterations << " iterations..." << std::endl;

    for (uint64_t i = start_step; i <= max_iterations; ++i) {
        sum = sum + term;
        big_udec factor(big_uint(i), big_uint(2 * i + 1));
        term = term * factor;

        if (i % 1000 == 0 || i == max_iterations) {
            sum.reduce_fraction();
            term.reduce_fraction();
            std::cout << "@ step " << i << " -> pi = " << sum.str(50) << std::endl;

            if (!checkpoint_file.empty()) {
                save_checkpoint(checkpoint_file, i, sum, term);
            }
        }
    }

    sum.reduce_fraction();
    std::cout << "\nFinal pi (" << max_iterations << " steps):\n" << sum.fractional_str() << std::endl;
    return 0;
}
