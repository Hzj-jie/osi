#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "../../../../interpreter/primitive/simulator.hpp"
#include "../../../../interpreter/primitive/parser.hpp"
#include "../../../../interpreter/primitive/bytecode.hpp"

using namespace primitive;

static bool read_stream(std::istream& is, std::vector<uint8_t>& out_bytes, std::string& out_str)
{
    out_bytes.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
    out_str.assign(reinterpret_cast<const char*>(out_bytes.data()), out_bytes.size());
    return !out_bytes.empty();
}

int main(int argc, char* argv[])
{
    std::vector<uint8_t> raw_bytes;
    std::string text_str;

    if (argc >= 2 && std::string(argv[1]) != "-")
    {
        std::ifstream ifs(argv[1], std::ios::binary);
        if (!ifs.is_open())
        {
            std::cerr << "Error: Cannot open file " << argv[1] << std::endl;
            return 1;
        }
        if (!read_stream(ifs, raw_bytes, text_str))
        {
            std::cerr << "Error: File is empty or cannot be read: " << argv[1] << std::endl;
            return 1;
        }
    }
    else
    {
        if (!read_stream(std::cin, raw_bytes, text_str))
        {
            std::cerr << "Error: Standard input is empty." << std::endl;
            return 1;
        }
    }

    std::vector<instruction> instructions;

    // Try binary bytecode format first
    if (!bytecode::import_bytes(raw_bytes, instructions))
    {
        // Fallback to text assembly parsing
        if (!parser::parse_program(text_str, instructions))
        {
            std::cerr << "Error: Failed to parse input as binary bytecode or text assembly." << std::endl;
            return 2;
        }
    }

    simulator sim;
    sim.load_instructions(std::move(instructions));

    bool ok = sim.run(SIZE_MAX); // Unlimited steps, matching osi.net execution
    if (!ok && !sim.reg().stop)
    {
        std::cerr << "VM halted with error (ip=" << sim.reg().ip << ")" << std::endl;
        return 3;
    }

    return 0;
}
