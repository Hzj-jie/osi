#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include "../../../math/big_udec.hpp"

using namespace osi::math;

bool save_checkpoint(const std::string& path, uint64_t step, const big_udec& sum) {
    std::ofstream ofs(path);
    if (!ofs.is_open()) return false;
    ofs << step << "\n";
    ofs << sum.fractional_str() << "\n";
    return true;
}

bool load_checkpoint(const std::string& path, uint64_t& step, big_udec& sum, big_udec& c) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return false;
    std::string line_step, line_sum;
    if (!std::getline(ifs, line_step) || !std::getline(ifs, line_sum)) return false;
    step = static_cast<uint64_t>(std::stoull(line_step));
    if (!big_udec::parse_fraction(line_sum, sum)) return false;

    c = big_udec(big_uint(1U), sum.denominator());
    return true;
}

int main(int argc, char* argv[]) {
    uint64_t max_iterations = 200;
    std::string checkpoint_file;
    std::string resume_file;
    uint64_t output_interval_sec = 3600; // 1 hour by default

    if (argc >= 2) {
        max_iterations = static_cast<uint64_t>(std::stoull(argv[1]));
    }
    if (argc >= 3) {
        checkpoint_file = argv[2];
    }
    if (argc >= 4) {
        resume_file = argv[3];
    }
    const char* env_interval = std::getenv("OUTPUT_INTERVAL_SEC");
    if (env_interval != nullptr) {
        output_interval_sec = static_cast<uint64_t>(std::stoull(env_interval));
    }
    if (argc >= 5) {
        output_interval_sec = static_cast<uint64_t>(std::stoull(argv[4]));
    }

    uint64_t step = 0;
    big_udec sum{1U};
    big_udec c{1U};
    uint64_t start_step = 1;

    if (!resume_file.empty()) {
        if (load_checkpoint(resume_file, step, sum, c)) {
            start_step = step + 1;
            std::cout << "Resumed e calculation from checkpoint step " << step << std::endl;
        } else {
            std::cerr << "Error: Failed to load resume checkpoint from " << resume_file << std::endl;
            return 1;
        }
    } else if (!checkpoint_file.empty()) {
        if (load_checkpoint(checkpoint_file, step, sum, c)) {
            start_step = step + 1;
            std::cout << "Auto-resuming e calculation from existing checkpoint step " << step << std::endl;
        }
    }

    std::cout << "Calculating e from step " << start_step << " up to " << max_iterations
              << " iterations (output interval: " << output_interval_sec << "s)..." << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    auto last_time = start_time;
    uint64_t last_step = start_step - 1;
    auto interval_duration = std::chrono::seconds(output_interval_sec);

    for (uint64_t i = start_step; i <= max_iterations; ++i) {
        c.divide(big_udec(i));
        sum.add(c);

        auto now = std::chrono::steady_clock::now();
        if (now - last_time >= interval_duration || i == max_iterations) {
            sum.reduce_fraction();
            c.reduce_fraction();

            now = std::chrono::steady_clock::now();
            double interval_sec = std::chrono::duration<double>(now - last_time).count();
            double total_sec = std::chrono::duration<double>(now - start_time).count();
            uint64_t interval_steps = i - last_step;
            double current_speed = interval_sec > 0.0 ? (interval_steps / interval_sec) : 0.0;
            double avg_speed = total_sec > 0.0 ? ((i - start_step + 1) / total_sec) : 0.0;
            double progress_pct = (max_iterations > 0) ? (static_cast<double>(i) / max_iterations * 100.0) : 100.0;

            std::cout << "@ step " << i << " / " << max_iterations
                      << " (" << std::fixed << std::setprecision(1) << progress_pct << "%)"
                      << " | Speed: " << std::setprecision(2) << current_speed << " steps/s"
                      << " (avg: " << avg_speed << " steps/s)"
                      << " | Elapsed: " << std::setprecision(2) << total_sec << "s";

            if (!checkpoint_file.empty()) {
                if (save_checkpoint(checkpoint_file, i, sum)) {
                    std::cout << " [Saved: " << checkpoint_file << "]";
                } else {
                    std::cout << " [Failed to save checkpoint]";
                }
            }
            std::cout << std::endl;

            last_time = now;
            last_step = i;
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    double total_sec = std::chrono::duration<double>(end_time - start_time).count();
    double avg_speed = total_sec > 0.0 ? ((max_iterations - start_step + 1) / total_sec) : 0.0;

    std::cout << "\nFinished calculation of e (" << (max_iterations - start_step + 1)
              << " steps completed in " << std::fixed << std::setprecision(2) << total_sec
              << "s, avg " << avg_speed << " steps/s)." << std::endl;
    if (!checkpoint_file.empty()) {
        std::cout << "Full result saved to checkpoint file: " << checkpoint_file << std::endl;
    }
    return 0;
}
