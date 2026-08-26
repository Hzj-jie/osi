#pragma once
#include "executor_case.hpp"
#include <string>

namespace logic_test
{
    class import_executor_case : public executor_case
    {
    protected:
        std::shared_ptr<primitive::interrupts> functions_;

        import_executor_case(const std::string& code, std::shared_ptr<primitive::interrupts> functions = nullptr)
            : functions_(std::move(functions))
        {
            es_builder_ = [code, this]() {
                std::vector<std::shared_ptr<osi::compiler::logic::instruction_gen>> es;
                osi::compiler::logic::importer imp(functions_);
                if (!imp.import(code, es))
                {
                    return std::vector<std::shared_ptr<osi::compiler::logic::instruction_gen>>();
                }
                return es;
            };
        }

        std::shared_ptr<primitive::interrupts> interrupts() override
        {
            return functions_;
        }
    };
}
