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

bool load_checkpoint(const std::string& path, uint64_t& step) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return false;
    std::string line_step;
    if (!std::getline(ifs, line_step)) return false;
    step = static_cast<uint64_t>(std::stoull(line_step));
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
    uint64_t start_step = 1;

    if (!resume_file.empty()) {
        if (load_checkpoint(resume_file, step)) {
            start_step = step + 1;
            std::cout << "Resumed e calculation from checkpoint step " << step << std::endl;
        } else {
            std::cerr << "Error: Failed to load resume checkpoint from " << resume_file << std::endl;
            return 1;
        }
    } else if (!checkpoint_file.empty()) {
        if (load_checkpoint(checkpoint_file, step)) {
            start_step = step + 1;
            std::cout << "Auto-resuming e calculation from existing checkpoint step " << step << std::endl;
        }
    }

    std::cout << "Calculating e from step " << start_step << " up to " << max_iterations
              << " iterations (output interval: " << output_interval_sec << "s)..." << std::endl;

    // Optimization: Instead of general fraction arithmetic sum.add(c) where denominators
    // cross-multiply exponentially and require massive GCD reductions, we exploit the
    // exact common denominator of the partial sum S_k = \sum_{j=0}^k 1/j! = N_k / k!.
    //
    // The recurrence is:
    //   N_{k+1} = (k+1) * N_k + 1
    //   D_{k+1} = (k+1) * D_k
    //
    // Every step is a single-pass O(limbs) scalar multiplication by a 64-bit integer,
    // avoiding O(L^2) BigInteger * BigInteger multiplication, division, and GCD in the loop.
    big_uint N(1ULL);
    big_uint D(1ULL);

    if (start_step > 1) {
        std::cout << "Fast-forwarding recurrence state to step " << (start_step - 1) << "..." << std::endl;
        auto ff_start = std::chrono::steady_clock::now();
        for (uint64_t k = 1; k < start_step; ++k) {
            N.multiply(k);
            N.add(big_uint(1ULL));
            D.multiply(k);
        }
        auto ff_end = std::chrono::steady_clock::now();
        double ff_sec = std::chrono::duration<double>(ff_end - ff_start).count();
        std::cout << "Fast-forwarded " << (start_step - 1) << " steps in " << ff_sec << "s." << std::endl;
    }

    auto start_time = std::chrono::steady_clock::now();
    auto last_time = start_time;
    uint64_t last_step = start_step - 1;
    auto interval_duration = std::chrono::seconds(output_interval_sec);

    for (uint64_t i = start_step; i <= max_iterations; ++i) {
        N.multiply(i);
        N.add(big_uint(1ULL));
        D.multiply(i);

        auto now = std::chrono::steady_clock::now();
        if (now - last_time >= interval_duration || i == max_iterations) {
            big_udec sum(N, D);
            sum.reduce_fraction();

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
