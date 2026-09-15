#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include "../../../../compiler/logic/importer.hpp"
#include "../../../../interpreter/primitive/bytecode.hpp"

namespace fs = std::filesystem;
using namespace osi::compiler::logic;
using namespace primitive;

static void print_usage(const char* prog)
{
    std::cout << "Usage: " << prog << " [options] [input_file] [output_file]\n"
              << "Options:\n"
              << "  -b, --binary         Output primitive binary bytecode (default if output ends with .bin)\n"
              << "  -t, --text           Output primitive text assembly (default)\n"
              << "  -o, --output <file>  Specify output file (default: stdout)\n"
              << "  -h, --help           Show this help message\n";
}

static bool read_stream(std::istream& is, std::string& out_str)
{
    out_str.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
    return !out_str.empty();
}

static bool has_extension(const std::string& path, const std::string& ext)
{
    if (path.size() < ext.size()) return false;
    return path.compare(path.size() - ext.size(), ext.size(), ext) == 0;
}

int main(int argc, char* argv[])
{
    std::string input_file;
    std::string output_file;
    bool binary_mode = false;
    bool binary_explicit = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            print_usage(argv[0]);
            return 0;
        }
        else if (arg == "-b" || arg == "--binary")
        {
            binary_mode = true;
            binary_explicit = true;
        }
        else if (arg == "-t" || arg == "--text")
        {
            binary_mode = false;
            binary_explicit = true;
        }
        else if (arg == "-o" || arg == "--output")
        {
            if (i + 1 < argc)
            {
                output_file = argv[++i];
            }
            else
            {
                std::cerr << "Error: " << arg << " requires an argument." << std::endl;
                return 1;
            }
        }
        else if (!arg.empty() && arg[0] == '-')
        {
            if (arg == "-")
            {
                if (input_file.empty()) input_file = "-";
                else if (output_file.empty()) output_file = "-";
            }
            else
            {
                std::cerr << "Error: Unknown option: " << arg << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        }
        else
        {
            if (input_file.empty())
            {
                input_file = arg;
            }
            else if (output_file.empty())
            {
                output_file = arg;
            }
            else
            {
                std::cerr << "Error: Unexpected positional argument: " << arg << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        }
    }

    if (!binary_explicit && !output_file.empty())
    {
        if (has_extension(output_file, ".bin"))
        {
            binary_mode = true;
        }
    }

    std::string content;
    std::string input_name;
    if (input_file.empty() || input_file == "-")
    {
        input_name = "<stdin>";
        if (!read_stream(std::cin, content))
        {
            std::cerr << "Error: Standard input is empty." << std::endl;
            return 1;
        }
    }
    else
    {
        input_name = input_file;
        std::ifstream ifs(input_file);
        if (!ifs.is_open())
        {
            std::cerr << "Error: Cannot open input file: " << input_file << std::endl;
            return 1;
        }
        if (!read_stream(ifs, content))
        {
            std::cerr << "Error: Input file is empty: " << input_file << std::endl;
            return 1;
        }
    }

    importer imp;
    std::vector<std::shared_ptr<instruction_gen>> es;
    if (!imp.import(content, es))
    {
        std::cerr << "Error: Failed to import logic code from " << input_name << std::endl;
        return 2;
    }

    if (binary_mode)
    {
        std::vector<instruction> insts;
        if (!importer::compile_to_instructions(es, insts))
        {
            std::cerr << "Error: Failed to compile logic code to primitive instructions." << std::endl;
            return 3;
        }
        std::vector<uint8_t> bytes = bytecode::export_bytes(insts);
        if (output_file.empty() || output_file == "-")
        {
            std::cout.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        }
        else
        {
            std::ofstream ofs(output_file, std::ios::binary);
            if (!ofs.is_open())
            {
                std::cerr << "Error: Cannot open output file: " << output_file << std::endl;
                return 4;
            }
            ofs.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        }
    }
    else
    {
        std::vector<std::string> lines;
        if (!importer::compile(es, lines))
        {
            std::cerr << "Error: Failed to compile logic code to primitive assembly." << std::endl;
            return 3;
        }
        if (output_file.empty() || output_file == "-")
        {
            for (const auto& line : lines)
            {
                std::cout << line << "\n";
            }
        }
        else
        {
            std::ofstream ofs(output_file);
            if (!ofs.is_open())
            {
                std::cerr << "Error: Cannot open output file: " << output_file << std::endl;
                return 4;
            }
            for (const auto& line : lines)
            {
                ofs << line << "\n";
            }
        }
    }

    return 0;
}
