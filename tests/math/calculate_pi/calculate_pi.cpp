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
    uint64_t max_iterations = 500;
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

    uint64_t step = 1;
    uint64_t start_step = 2;
    big_uint N(8ULL);
    big_uint D_sum(3ULL);
    big_uint T(4ULL);
    big_uint D_term(15ULL);
    bool is_binary = false;
    pi_checkpoint_state chk_state;

    std::string load_target = !resume_file.empty() ? resume_file : checkpoint_file;
    if (!load_target.empty()) {
        std::ifstream test_open(load_target);
        if (test_open.is_open()) {
            test_open.close();
            if (load_pi_checkpoint(load_target, chk_state, is_binary, /*parse_numbers=*/false)) {
                step = chk_state.step;
                start_step = step + 1;
                if (is_binary) {
                    N = std::move(chk_state.N);
                    D_sum = std::move(chk_state.D_sum);
                    T = std::move(chk_state.T);
                    D_term = std::move(chk_state.D_term);
                    std::cout << "Resumed pi calculation from binary checkpoint step " << step
                              << " (exact recurrence state restored directly)." << std::endl;
                } else {
                    std::cout << "Resumed pi calculation from text checkpoint step " << step
                              << " (fast-forwarding recurrence state to step " << step << ")..." << std::endl;
                    if (start_step > 2) {
                        auto ff_start = std::chrono::steady_clock::now();
                        for (uint64_t k = 3; k < start_step; ++k) {
                            N.multiply(2 * k - 1);
                            N.add(T);
                            D_sum = D_term;

                            T.multiply(k);
                            D_term.multiply(2 * k + 1);
                        }
                        auto ff_end = std::chrono::steady_clock::now();
                        double ff_sec = std::chrono::duration<double>(ff_end - ff_start).count();
                        std::cout << "Fast-forwarded " << (start_step - 2) << " steps in " << ff_sec << "s." << std::endl;
                    }
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

    std::cout << "Calculating pi (Newton arctangent series) from step " << start_step
              << " up to " << max_iterations << " iterations (output interval: "
              << output_interval_sec << "s)..." << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    auto last_time = start_time;
    uint64_t last_step = start_step - 1;
    auto interval_duration = std::chrono::seconds(output_interval_sec);

    for (uint64_t i = start_step; i <= max_iterations; ++i) {
        if (i > 2) {
            N.multiply(2 * i - 1);
            N.add(T);
            D_sum = D_term;

            T.multiply(i);
            D_term.multiply(2 * i + 1);
        }

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
                if (save_pi_checkpoint_binary(checkpoint_file, i, N, D_sum, T, D_term)) {
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

    std::cout << "\nFinished calculation of pi (" << (max_iterations - start_step + 1)
              << " steps completed in " << std::fixed << std::setprecision(2) << total_sec
              << "s, avg " << avg_speed << " steps/s)." << std::endl;
    if (!checkpoint_file.empty()) {
        std::cout << "Checkpoint saved to: " << checkpoint_file << std::endl;
        std::cout << "To export human-readable fraction string, run:\n"
                  << "  convert_checkpoint " << checkpoint_file << " <output_text_file>" << std::endl;
    }
    return 0;
}
