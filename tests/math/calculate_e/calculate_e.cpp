#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <cstring>
#include "../../../math/big_udec.hpp"

using namespace osi::math;

static bool save_checkpoint(const std::string& path, uint64_t step, const big_uint& N, const big_uint& D) {
    std::string tmp_path = path + ".tmp";
    std::ofstream ofs(tmp_path, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open()) return false;
    ofs.write(reinterpret_cast<const char*>(&step), sizeof(step));
    N.write_binary(ofs);
    D.write_binary(ofs);
    ofs.close();
    if (!ofs.good()) return false;
    return std::rename(tmp_path.c_str(), path.c_str()) == 0;
}

static bool load_checkpoint(const std::string& path, uint64_t& step, big_uint& N, big_uint& D, bool& is_binary) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) return false;
    char magic[8];
    if (ifs.read(magic, 8) && std::memcmp(magic, "OSICKPT1", 8) == 0) {
        uint32_t type = 0;
        if (!ifs.read(reinterpret_cast<char*>(&type), sizeof(type))) return false;
        if (!ifs.read(reinterpret_cast<char*>(&step), sizeof(step))) return false;
        if (!N.read_binary(ifs) || !D.read_binary(ifs)) return false;
        is_binary = true;
        return true;
    }
    ifs.clear();
    ifs.seekg(0);
    char header[16] = {0};
    std::streamsize bytes_read = ifs.read(header, sizeof(header)).gcount();
    if (bytes_read == 0) return false;
    bool has_null = false;
    for (std::streamsize i = 0; i < bytes_read; ++i) {
        if (header[i] == '\0') {
            has_null = true;
            break;
        }
    }
    if (has_null) {
        ifs.clear();
        ifs.seekg(0);
        if (!ifs.read(reinterpret_cast<char*>(&step), sizeof(step))) return false;
        if (!N.read_binary(ifs) || !D.read_binary(ifs)) return false;
        is_binary = true;
        return true;
    }

    // Text checkpoint fallback
    ifs.close();
    std::ifstream tifs(path);
    if (!tifs.is_open()) return false;
    std::string line_step;
    if (!std::getline(tifs, line_step)) return false;
    step = static_cast<uint64_t>(std::stoull(line_step));
    is_binary = false;
    return true;
}

static int dump_to_decimal(const std::string& input_file, const std::string& output_file) {
    std::ifstream ifs(input_file, std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "Error: Cannot open " << input_file << std::endl;
        return 1;
    }
    uint64_t step = 0;
    char magic[8];
    if (ifs.read(magic, 8) && std::memcmp(magic, "OSICKPT1", 8) == 0) {
        uint32_t type = 0;
        ifs.read(reinterpret_cast<char*>(&type), sizeof(type));
        ifs.read(reinterpret_cast<char*>(&step), sizeof(step));
    } else {
        ifs.clear();
        ifs.seekg(0);
        if (!ifs.read(reinterpret_cast<char*>(&step), sizeof(step))) {
            std::cerr << "Error: Cannot read step from " << input_file << std::endl;
            return 1;
        }
    }

    std::ostream* os = &std::cout;
    std::ofstream ofs;
    if (!output_file.empty()) {
        ofs.open(output_file);
        if (!ofs.is_open()) {
            std::cerr << "Error: Cannot open output file " << output_file << std::endl;
            return 1;
        }
        os = &ofs;
    }

    *os << step << "\n";
    if (!big_udec::binary_to_decimal(ifs, *os)) {
        std::cerr << "Error: Failed to deserialize big_udec from " << input_file << std::endl;
        return 1;
    }
    *os << "\n";
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc >= 2) {
        std::string arg1 = argv[1];
        if (arg1 == "--dump" || arg1 == "--to-decimal") {
            if (argc < 3) {
                std::cerr << "Usage: " << argv[0] << " --dump <checkpoint_file> [output_file]" << std::endl;
                return 1;
            }
            std::string input_file = argv[2];
            std::string output_file = (argc >= 4) ? argv[3] : "";
            return dump_to_decimal(input_file, output_file);
        }
    }

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

    std::string load_target = !resume_file.empty() ? resume_file : checkpoint_file;
    if (!load_target.empty()) {
        std::ifstream test_open(load_target);
        if (test_open.is_open()) {
            test_open.close();
            if (load_checkpoint(load_target, step, N, D, is_binary)) {
                start_step = step + 1;
                if (is_binary) {
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
                if (save_checkpoint(checkpoint_file, i, N, D)) {
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
                  << "  " << argv[0] << " --dump " << checkpoint_file << " <output_text_file>" << std::endl;
    }
    return 0;
}

