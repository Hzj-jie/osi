#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <vector>
#include "../../math/big_udec.hpp"

namespace osi {
namespace math {

// Checkpoint file format:
// Binary files begin with the 8-byte magic: "OSICKPT1"
// followed by a uint32_t type identifier:
//   1 = CHECKPOINT_TYPE_E (stores step, N, D)
//   2 = CHECKPOINT_TYPE_PI (stores step, N, D_sum, T, D_term)
// followed by uint64_t step
// followed by the binary limb data for each big_uint.
//
// Legacy/text checkpoint files do not start with "OSICKPT1".
// They have:
// Line 1: step
// Line 2: sum_n / sum_d
// Line 3 (optional, for pi): term_n / term_d

constexpr char CHECKPOINT_MAGIC[8] = {'O', 'S', 'I', 'C', 'K', 'P', 'T', '1'};
constexpr uint32_t CHECKPOINT_TYPE_E = 1;
constexpr uint32_t CHECKPOINT_TYPE_PI = 2;

inline bool is_binary_checkpoint(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) return false;
    char magic[8];
    if (!ifs.read(magic, 8)) return false;
    return std::memcmp(magic, CHECKPOINT_MAGIC, 8) == 0;
}

// State for e calculation: S_k = N / D, where D = k!
struct e_checkpoint_state {
    uint64_t step{0};
    big_uint N{1ULL};
    big_uint D{1ULL};
};

inline bool save_e_checkpoint_binary(const std::string& path, uint64_t step, const big_uint& N, const big_uint& D) {
    std::string tmp_path = path + ".tmp";
    std::ofstream ofs(tmp_path, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open()) return false;
    ofs.write(CHECKPOINT_MAGIC, 8);
    uint32_t type = CHECKPOINT_TYPE_E;
    ofs.write(reinterpret_cast<const char*>(&type), sizeof(type));
    ofs.write(reinterpret_cast<const char*>(&step), sizeof(step));
    N.write_binary(ofs);
    D.write_binary(ofs);
    ofs.close();
    if (!ofs.good()) return false;
    return std::rename(tmp_path.c_str(), path.c_str()) == 0;
}

inline bool load_e_checkpoint(const std::string& path, e_checkpoint_state& state, bool& is_binary, bool parse_numbers = true) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) return false;
    char magic[8];
    if (ifs.read(magic, 8) && std::memcmp(magic, CHECKPOINT_MAGIC, 8) == 0) {
        uint32_t type = 0;
        if (!ifs.read(reinterpret_cast<char*>(&type), sizeof(type))) return false;
        if (type != CHECKPOINT_TYPE_E) return false;
        if (!ifs.read(reinterpret_cast<char*>(&state.step), sizeof(state.step))) return false;
        if (!state.N.read_binary(ifs)) return false;
        if (!state.D.read_binary(ifs)) return false;
        is_binary = true;
        return true;
    }
    // Text checkpoint fallback
    ifs.close();
    std::ifstream tifs(path);
    if (!tifs.is_open()) return false;
    std::string line_step, line_sum;
    if (!std::getline(tifs, line_step)) return false;
    state.step = static_cast<uint64_t>(std::stoull(line_step));
    is_binary = false;
    if (!parse_numbers) return true;

    if (std::getline(tifs, line_sum) && !line_sum.empty()) {
        size_t slash = line_sum.find('/');
        if (slash != std::string::npos) {
            state.N = big_uint(line_sum.substr(0, slash));
            state.D = big_uint(line_sum.substr(slash + 1));
        } else {
            state.N = big_uint(line_sum);
            state.D = big_uint(1ULL);
        }
    }
    return true;
}

// State for pi calculation:
// sum = N / D_sum, where D_sum = (2*step - 1)!!
// term = T / D_term, where D_term = (2*step + 1)!!
struct pi_checkpoint_state {
    uint64_t step{0};
    big_uint N{8ULL};
    big_uint D_sum{3ULL};
    big_uint T{4ULL};
    big_uint D_term{15ULL};
};

inline bool save_pi_checkpoint_binary(const std::string& path, uint64_t step,
                                     const big_uint& N, const big_uint& D_sum,
                                     const big_uint& T, const big_uint& D_term) {
    std::string tmp_path = path + ".tmp";
    std::ofstream ofs(tmp_path, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open()) return false;
    ofs.write(CHECKPOINT_MAGIC, 8);
    uint32_t type = CHECKPOINT_TYPE_PI;
    ofs.write(reinterpret_cast<const char*>(&type), sizeof(type));
    ofs.write(reinterpret_cast<const char*>(&step), sizeof(step));
    N.write_binary(ofs);
    D_sum.write_binary(ofs);
    T.write_binary(ofs);
    D_term.write_binary(ofs);
    ofs.close();
    if (!ofs.good()) return false;
    return std::rename(tmp_path.c_str(), path.c_str()) == 0;
}

inline bool load_pi_checkpoint(const std::string& path, pi_checkpoint_state& state, bool& is_binary, bool parse_numbers = true) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) return false;
    char magic[8];
    if (ifs.read(magic, 8) && std::memcmp(magic, CHECKPOINT_MAGIC, 8) == 0) {
        uint32_t type = 0;
        if (!ifs.read(reinterpret_cast<char*>(&type), sizeof(type))) return false;
        if (type != CHECKPOINT_TYPE_PI) return false;
        if (!ifs.read(reinterpret_cast<char*>(&state.step), sizeof(state.step))) return false;
        if (!state.N.read_binary(ifs)) return false;
        if (!state.D_sum.read_binary(ifs)) return false;
        if (!state.T.read_binary(ifs)) return false;
        if (!state.D_term.read_binary(ifs)) return false;
        is_binary = true;
        return true;
    }
    // Text checkpoint fallback
    ifs.close();
    std::ifstream tifs(path);
    if (!tifs.is_open()) return false;
    std::string line_step, line_sum, line_term;
    if (!std::getline(tifs, line_step)) return false;
    state.step = static_cast<uint64_t>(std::stoull(line_step));
    is_binary = false;
    if (!parse_numbers) return true;

    if (std::getline(tifs, line_sum) && !line_sum.empty()) {
        size_t slash = line_sum.find('/');
        if (slash != std::string::npos) {
            state.N = big_uint(line_sum.substr(0, slash));
            state.D_sum = big_uint(line_sum.substr(slash + 1));
        } else {
            state.N = big_uint(line_sum);
            state.D_sum = big_uint(1ULL);
        }
    }
    if (std::getline(tifs, line_term) && !line_term.empty()) {
        size_t slash = line_term.find('/');
        if (slash != std::string::npos) {
            state.T = big_uint(line_term.substr(0, slash));
            state.D_term = big_uint(line_term.substr(slash + 1));
        } else {
            state.T = big_uint(line_term);
            state.D_term = big_uint(1ULL);
        }
    }
    return true;
}

} // namespace math
} // namespace osi
