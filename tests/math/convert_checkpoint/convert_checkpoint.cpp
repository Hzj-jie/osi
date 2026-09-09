#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "../checkpoint.hpp"

using namespace osi::math;

void print_usage(const char* prog) {
    std::cout << "Usage:\n"
              << "  " << prog << " <input_checkpoint> [output_file] [options]\n\n"
              << "Options:\n"
              << "  --reduce         Perform full fraction reduction (GCD) before text export\n"
              << "  --fast-reduce    Perform fast power-of-2 fraction reduction before text export\n"
              << "  --info           Print checkpoint metadata (type, step, limbs, digits)\n"
              << "  --to-binary <e|pi> Convert a legacy text checkpoint to binary format\n"
              << "\nExamples:\n"
              << "  " << prog << " e.bin e_human.txt\n"
              << "  " << prog << " pi.bin pi_human.txt --fast-reduce\n"
              << "  " << prog << " e.txt e.bin --to-binary e\n"
              << "  " << prog << " pi.bin --info\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string input_path = argv[1];
    std::string output_path;
    bool do_reduce = false;
    bool do_fast_reduce = false;
    bool info_only = false;
    bool to_binary = false;
    std::string to_binary_type = "e";

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--reduce") {
            do_reduce = true;
        } else if (arg == "--fast-reduce") {
            do_fast_reduce = true;
        } else if (arg == "--info") {
            info_only = true;
        } else if (arg == "--to-binary") {
            to_binary = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                to_binary_type = argv[++i];
            }
        } else if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else if (output_path.empty() && arg[0] != '-') {
            output_path = arg;
        }
    }

    bool is_binary = is_binary_checkpoint(input_path);

    if (to_binary) {
        if (output_path.empty()) {
            std::cerr << "Error: --to-binary requires an output file path." << std::endl;
            return 1;
        }
        if (to_binary_type == "e") {
            e_checkpoint_state state;
            bool was_binary = false;
            if (!load_e_checkpoint(input_path, state, was_binary)) {
                std::cerr << "Error: Failed to load e checkpoint from " << input_path << std::endl;
                return 1;
            }
            if (!save_e_checkpoint_binary(output_path, state.step, state.N, state.D)) {
                std::cerr << "Error: Failed to write binary checkpoint to " << output_path << std::endl;
                return 1;
            }
            std::cout << "Successfully converted e text checkpoint to binary: " << output_path
                      << " (step: " << state.step << ")" << std::endl;
            return 0;
        } else if (to_binary_type == "pi") {
            pi_checkpoint_state state;
            bool was_binary = false;
            if (!load_pi_checkpoint(input_path, state, was_binary)) {
                std::cerr << "Error: Failed to load pi checkpoint from " << input_path << std::endl;
                return 1;
            }
            if (!save_pi_checkpoint_binary(output_path, state.step, state.N, state.D_sum, state.T, state.D_term)) {
                std::cerr << "Error: Failed to write binary checkpoint to " << output_path << std::endl;
                return 1;
            }
            std::cout << "Successfully converted pi text checkpoint to binary: " << output_path
                      << " (step: " << state.step << ")" << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Unknown --to-binary type: " << to_binary_type << " (expected 'e' or 'pi')" << std::endl;
            return 1;
        }
    }

    if (!is_binary) {
        std::cout << "Input file is a text checkpoint: " << input_path << std::endl;
        std::ifstream ifs(input_path);
        if (!ifs.is_open()) {
            std::cerr << "Error: Failed to open " << input_path << std::endl;
            return 1;
        }
        std::string line1;
        if (std::getline(ifs, line1)) {
            std::cout << "Checkpoint step: " << line1 << std::endl;
        }
        if (info_only) return 0;
        std::cout << "Input is already in text format. Use --to-binary to convert to binary format." << std::endl;
        return 0;
    }

    // Binary checkpoint: inspect header
    std::ifstream ifs(input_path, std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "Error: Cannot open " << input_path << std::endl;
        return 1;
    }
    char magic[8];
    ifs.read(magic, 8);
    uint32_t type = 0;
    ifs.read(reinterpret_cast<char*>(&type), sizeof(type));
    ifs.close();

    if (type == CHECKPOINT_TYPE_E) {
        e_checkpoint_state state;
        bool b = false;
        if (!load_e_checkpoint(input_path, state, b)) {
            std::cerr << "Error: Failed to read e binary checkpoint from " << input_path << std::endl;
            return 1;
        }

        if (info_only) {
            std::cout << "=== e Checkpoint Info ===" << "\n"
                      << "Type: e (partial sum N/k!)\n"
                      << "Step: " << state.step << "\n"
                      << "Numerator limbs: " << state.N.limb_count() << " (~" << (state.N.limb_count() * 19) << " digits)\n"
                      << "Denominator limbs: " << state.D.limb_count() << " (~" << (state.D.limb_count() * 19) << " digits)\n";
            return 0;
        }

        std::cout << "Converting e checkpoint (step " << state.step << ") to fractional text..." << std::endl;
        auto t0 = std::chrono::steady_clock::now();
        big_udec sum(state.N, state.D);
        if (do_reduce) {
            std::cout << "Performing fraction reduction (GCD)..." << std::endl;
            sum.reduce_fraction();
        } else if (do_fast_reduce) {
            std::cout << "Performing fast fraction reduction..." << std::endl;
            sum.fast_reduce_fraction();
        }

        std::string out_str = sum.fractional_str();
        auto t1 = std::chrono::steady_clock::now();
        double sec = std::chrono::duration<double>(t1 - t0).count();
        std::cout << "Formatted fractional string in " << sec << "s." << std::endl;

        if (!output_path.empty()) {
            std::ofstream ofs(output_path);
            if (!ofs.is_open()) {
                std::cerr << "Error: Cannot open output file " << output_path << std::endl;
                return 1;
            }
            ofs << state.step << "\n";
            ofs << out_str << "\n";
            std::cout << "Saved human-readable checkpoint to " << output_path << std::endl;
        } else {
            std::cout << state.step << "\n" << out_str << std::endl;
        }
    } else if (type == CHECKPOINT_TYPE_PI) {
        pi_checkpoint_state state;
        bool b = false;
        if (!load_pi_checkpoint(input_path, state, b)) {
            std::cerr << "Error: Failed to read pi binary checkpoint from " << input_path << std::endl;
            return 1;
        }

        if (info_only) {
            std::cout << "=== pi Checkpoint Info ===" << "\n"
                      << "Type: pi (Newton series)\n"
                      << "Step: " << state.step << "\n"
                      << "Sum N limbs: " << state.N.limb_count() << " (~" << (state.N.limb_count() * 19) << " digits)\n"
                      << "Sum D limbs: " << state.D_sum.limb_count() << "\n"
                      << "Term T limbs: " << state.T.limb_count() << "\n"
                      << "Term D limbs: " << state.D_term.limb_count() << "\n";
            return 0;
        }

        std::cout << "Converting pi checkpoint (step " << state.step << ") to fractional text..." << std::endl;
        auto t0 = std::chrono::steady_clock::now();
        big_udec sum(state.N, state.D_sum);
        big_udec term(state.T, state.D_term);
        if (do_reduce) {
            std::cout << "Performing fraction reduction (GCD)..." << std::endl;
            sum.reduce_fraction();
            term.reduce_fraction();
        } else if (do_fast_reduce) {
            std::cout << "Performing fast fraction reduction..." << std::endl;
            sum.fast_reduce_fraction();
            term.fast_reduce_fraction();
        }

        std::string sum_str = sum.fractional_str();
        std::string term_str = term.fractional_str();
        auto t1 = std::chrono::steady_clock::now();
        double sec = std::chrono::duration<double>(t1 - t0).count();
        std::cout << "Formatted fractional strings in " << sec << "s." << std::endl;

        if (!output_path.empty()) {
            std::ofstream ofs(output_path);
            if (!ofs.is_open()) {
                std::cerr << "Error: Cannot open output file " << output_path << std::endl;
                return 1;
            }
            ofs << state.step << "\n";
            ofs << sum_str << "\n";
            ofs << term_str << "\n";
            std::cout << "Saved human-readable checkpoint to " << output_path << std::endl;
        } else {
            std::cout << state.step << "\n" << sum_str << "\n" << term_str << std::endl;
        }
    } else {
        std::cerr << "Error: Unknown binary checkpoint type: " << type << std::endl;
        return 1;
    }

    return 0;
}
