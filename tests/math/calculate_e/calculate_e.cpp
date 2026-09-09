#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include "../checkpoint.hpp"

using namespace osi::math;

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
    big_uint N(1ULL);
    big_uint D(1ULL);
    bool is_binary = false;
    e_checkpoint_state chk_state;

    std::string load_target = !resume_file.empty() ? resume_file : checkpoint_file;
    if (!load_target.empty()) {
        std::ifstream test_open(load_target);
        if (test_open.is_open()) {
            test_open.close();
            if (load_e_checkpoint(load_target, chk_state, is_binary, /*parse_numbers=*/false)) {
                step = chk_state.step;
                start_step = step + 1;
                if (is_binary) {
                    N = std::move(chk_state.N);
                    D = std::move(chk_state.D);
                    std::cout << "Resumed e calculation from binary checkpoint step " << step
                              << " (exact recurrence state restored directly)." << std::endl;
                } else {
                    std::cout << "Resumed e calculation from text checkpoint step " << step
                              << " (fast-forwarding recurrence state to step " << step << ")..." << std::endl;
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
            } else if (!resume_file.empty()) {
                std::cerr << "Error: Failed to load resume checkpoint from " << resume_file << std::endl;
                return 1;
            }
        } else if (!resume_file.empty()) {
            std::cerr << "Error: Failed to open resume checkpoint file " << resume_file << std::endl;
            return 1;
        }
    }

    std::cout << "Calculating e from step " << start_step << " up to " << max_iterations
              << " iterations (output interval: " << output_interval_sec << "s)..." << std::endl;

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
                if (save_e_checkpoint_binary(checkpoint_file, i, N, D)) {
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
        std::cout << "Checkpoint saved to: " << checkpoint_file << std::endl;
        std::cout << "To export human-readable fraction string, run:\n"
                  << "  convert_checkpoint " << checkpoint_file << " <output_text_file>" << std::endl;
    }
    return 0;
}
