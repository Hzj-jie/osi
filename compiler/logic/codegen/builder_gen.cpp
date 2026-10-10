#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::string trim(const std::string& s)
{
    const size_t first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    const size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

static std::vector<std::string> split_tabs(const std::string& s)
{
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, '\t'))
    {
        if (!item.empty()) out.push_back(item);
    }
    return out;
}

static std::string join(const std::vector<std::string>& v, const std::string& sep)
{
    std::string out;
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0) out += sep;
        out += v[i];
    }
    return out;
}

int main()
{
    std::cout << "// This file is generated from definition.txt. Do not edit.\n"
              << "#pragma once\n"
              << "#include <string>\n"
              << "#include <vector>\n"
              << "#include <functional>\n"
              << "#include <cstdint>\n"
              << "#include <utility>\n"
              << "#include \"../../app_info/assert.hpp\"\n"
              << "#include \"../../interpreter/primitive/data_block.hpp\"\n"
              << "#include \"logic_writer.hpp\"\n\n"
              << "namespace osi\n"
              << "{\n"
              << "    namespace compiler\n"
              << "    {\n"
              << "        namespace logic\n"
              << "        {\n"
              << "            namespace builders\n"
              << "            {\n";

    int keys_count = 0;
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::string s = trim(line);
        if (s.empty() || s[0] == '#') continue;
        std::vector<std::string> ss = split_tabs(s);
        if (ss.empty()) continue;
        keys_count++;

        std::vector<std::string> fields;
        std::vector<std::string> ctor_params;
        std::vector<std::string> of_params;
        std::vector<std::string> inits;
        std::vector<std::string> asserts;
        std::vector<std::string> passed_params;
        std::vector<std::string> append_params;

        for (size_t i = 1; i < ss.size(); ++i)
        {
            std::string idx = std::to_string(i);
            std::string name = ss[i] + "_" + idx;
            std::string field_type;
            std::string param_type;
            if (ss[i] == "string")
            {
                field_type = "std::string";
                param_type = "const std::string&";
                asserts.push_back("assert(!" + name + ".empty());");
            }
            else if (ss[i] == "uint")
            {
                field_type = "uint32_t";
                param_type = "uint32_t";
            }
            else if (ss[i] == "data_block")
            {
                field_type = "primitive::data_block";
                param_type = "const primitive::data_block&";
            }
            else if (ss[i] == "parameters")
            {
                field_type = "std::vector<std::string>";
                param_type = "const std::vector<std::string>&";
            }
            else if (ss[i] == "typed_parameters")
            {
                field_type = "std::vector<std::pair<std::string, std::string>>";
                param_type = "const std::vector<std::pair<std::string, std::string>>&";
            }
            else if (ss[i] == "paragraph")
            {
                field_type = "std::function<bool(logic_writer&)>";
                param_type = "const std::function<bool(logic_writer&)>&";
                asserts.push_back("assert(" + name + " != nullptr);");
            }

            if (!field_type.empty())
            {
                fields.push_back(field_type + " " + name + ";");
                ctor_params.push_back(param_type + " p_" + name);
                of_params.push_back(param_type + " " + name);
                inits.push_back(name + "(p_" + name + ")");
                passed_params.push_back(name);
                if (ss[i] == "paragraph")
                {
                    append_params.push_back("\"{\"");
                }
                else if (ss[i] == "parameters" || ss[i] == "typed_parameters")
                {
                    append_params.push_back("\"(\"");
                }
                append_params.push_back(name);
                if (ss[i] == "paragraph")
                {
                    append_params.push_back("\"}\"");
                }
                else if (ss[i] == "parameters" || ss[i] == "typed_parameters")
                {
                    append_params.push_back("\")\"");
                }
            }
            else
            {
                append_params.push_back("\"" + ss[i] + "\"");
            }
        }

        std::string builder_name = ss[0] + "_builder_" + std::to_string(keys_count);
        std::cout << "                struct " << builder_name << "\n"
                  << "                {\n";
        for (const auto& f : fields)
        {
            std::cout << "                    " << f << "\n";
        }
        std::cout << "\n";
        if (!fields.empty())
        {
            std::cout << "                    " << builder_name << "(" << join(ctor_params, ", ") << ")\n"
                      << "                        : " << join(inits, ", ") << "\n"
                      << "                    {\n";
            for (const auto& a : asserts)
            {
                std::cout << "                        " << a << "\n";
            }
            std::cout << "                    }\n\n";
        }
        else
        {
            std::cout << "                    " << builder_name << "() = default;\n\n";
        }
        std::cout << "                    bool to(logic_writer& o) const\n"
                  << "                    {\n"
                  << "                        return o.append(\"" << ss[0] << "\") &&\n";
        for (const auto& ap : append_params)
        {
            std::cout << "                               o.append(" << ap << ") &&\n";
        }
        std::cout << "                               o.append(\"\\n\");\n"
                  << "                    }\n"
                  << "                };\n\n"
                  << "                inline " << builder_name << " of_" << ss[0] << "(" << join(of_params, ", ") << ")\n"
                  << "                {\n"
                  << "                    return " << builder_name << "(" << join(passed_params, ", ") << ");\n"
                  << "                }\n\n";
    }

    std::cout << "            }\n"
              << "        }\n"
              << "    }\n"
              << "}\n";
    return 0;
}
