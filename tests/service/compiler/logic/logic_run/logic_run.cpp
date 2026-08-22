#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include "../../../../../service/compiler/logic/importer.hpp"
#include "../../../../../primitive/simulator.hpp"

namespace fs = std::filesystem;
using namespace osi::compiler::logic;
using namespace primitive;

static bool read_stream(std::istream& is, std::string& out_str)
{
    out_str.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
    return !out_str.empty();
}

static bool run_logic(const std::string& content, const std::string& source_name)
{
    importer imp;
    simulator sim;
    if (!imp.import(content, sim))
    {
        std::cerr << "Error: Failed to import logic code from " << source_name << std::endl;
        return false;
    }

    bool ok = sim.run(SIZE_MAX);
    if (!ok && !sim.reg().stop)
    {
        std::cerr << "Error: VM halted with error (ip=" << sim.reg().ip << ") in " << source_name << std::endl;
        return false;
    }
    return true;
}

static bool run_file(const fs::path& p)
{
    std::ifstream ifs(p);
    if (!ifs.is_open())
    {
        std::cerr << "Error: Cannot open file " << p << std::endl;
        return false;
    }
    std::string content;
    if (!read_stream(ifs, content))
    {
        std::cerr << "Error: File is empty or cannot be read: " << p << std::endl;
        return false;
    }
    return run_logic(content, p.string());
}

static bool run_path(const fs::path& p)
{
    if (fs::is_directory(p))
    {
        std::vector<fs::path> files;
        for (const auto& entry : fs::directory_iterator(p))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".logic")
            {
                files.push_back(entry.path());
            }
        }
        std::sort(files.begin(), files.end());
        bool all_ok = true;
        for (const auto& file : files)
        {
            if (!run_file(file))
            {
                all_ok = false;
            }
        }
        return all_ok;
    }
    else
    {
        return run_file(p);
    }
}

int main(int argc, char* argv[])
{
    if (argc <= 1 || (argc == 2 && std::string(argv[1]) == "-"))
    {
        std::string content;
        if (!read_stream(std::cin, content))
        {
            std::cerr << "Error: Standard input is empty." << std::endl;
            return 1;
        }
        return run_logic(content, "<stdin>") ? 0 : 2;
    }

    bool all_ok = true;
    for (int i = 1; i < argc; ++i)
    {
        fs::path p(argv[i]);
        if (!run_path(p))
        {
            all_ok = false;
        }
    }

    return all_ok ? 0 : 3;
}
