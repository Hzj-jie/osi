#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include "../../../service/math/big_udec.hpp"

using namespace osi::math;

struct checkpoint_e {
    uint32_t step = 0;
    big_udec sum{1U};
    big_udec cur{1U};
};

bool save_checkpoint(const std::string& path, uint32_t step, const big_udec& sum, const big_udec& cur) {
    std::ofstream ofs(path);
    if (!ofs.is_open()) return false;
    ofs << step << "\n";
    ofs << sum.fractional_str() << "\n";
    ofs << cur.fractional_str() << "\n";
    return true;
}

bool load_checkpoint(const std::string& path, checkpoint_e& chk) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return false;
    std::string line_step, line_sum, line_cur;
    if (!std::getline(ifs, line_step) || !std::getline(ifs, line_sum) || !std::getline(ifs, line_cur)) return false;
    chk.step = static_cast<uint32_t>(std::stoul(line_step));
    if (!big_udec::parse_fraction(line_sum, chk.sum)) return false;
    if (!big_udec::parse_fraction(line_cur, chk.cur)) return false;
    return true;
}

int main(int argc, char* argv[]) {
    uint32_t max_iterations = 200;
    std::string checkpoint_file;
    std::string resume_file;

    if (argc >= 2) {
        max_iterations = static_cast<uint32_t>(std::stoul(argv[1]));
    }
    if (argc >= 3) {
        checkpoint_file = argv[2];
    }
    if (argc >= 4) {
        resume_file = argv[3];
    }

    checkpoint_e chk;
    uint32_t start_step = 1;

    if (!resume_file.empty()) {
        if (load_checkpoint(resume_file, chk)) {
            start_step = chk.step + 1;
            std::cout << "Resumed e calculation from checkpoint step " << chk.step << std::endl;
        } else {
            std::cerr << "Error: Failed to load resume checkpoint from " << resume_file << std::endl;
            return 1;
        }
    } else if (!checkpoint_file.empty()) {
        if (load_checkpoint(checkpoint_file, chk)) {
            start_step = chk.step + 1;
            std::cout << "Auto-resuming e calculation from existing checkpoint step " << chk.step << std::endl;
        }
    }

    big_udec sum = chk.sum;
    big_udec cur = chk.cur;

    std::cout << "Calculating e from step " << start_step << " up to " << max_iterations << " iterations..." << std::endl;

    for (uint32_t i = start_step; i <= max_iterations; ++i) {
        cur = cur / big_udec(i);
        sum = sum + cur;

        if (i % 50 == 0 || i == max_iterations) {
            sum.reduce_fraction();
            cur.reduce_fraction();
            std::cout << "@ step " << i << " -> e = " << sum.str(50) << std::endl;

            if (!checkpoint_file.empty()) {
                save_checkpoint(checkpoint_file, i, sum, cur);
            }
        }
    }

    sum.reduce_fraction();
    std::cout << "\nFinal e (" << max_iterations << " steps):\n" << sum.fractional_str() << std::endl;
    return 0;
}
