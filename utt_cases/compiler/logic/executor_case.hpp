#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "../../../compiler/logic/statements.hpp"
#include "../../../compiler/logic/importer.hpp"
#include "../../../interpreter/primitive/simulator.hpp"
#include "../../../interpreter/primitive/interrupts.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace logic_test
{
    class executor_case : public icase
    {
    protected:
        std::function<std::vector<std::shared_ptr<osi::compiler::logic::instruction_gen>>()> es_builder_;

        template <typename... Args>
        explicit executor_case(std::shared_ptr<osi::compiler::logic::instruction_gen> first, Args&&... rest)
        {
            std::vector<std::shared_ptr<osi::compiler::logic::instruction_gen>> es;
            es.push_back(std::move(first));
            (es.push_back(std::forward<Args>(rest)), ...);
            es_builder_ = [es]() { return es; };
        }

        explicit executor_case(std::function<std::vector<std::shared_ptr<osi::compiler::logic::instruction_gen>>()> es_builder)
            : es_builder_(std::move(es_builder)) {}

        executor_case() = default;

        virtual void check_result(primitive::simulator& e) {}
        virtual std::shared_ptr<primitive::interrupts> interrupts() { return nullptr; }

    public:
        bool run() override
        {
            auto ext = interrupts();
            primitive::simulator e(ext ? *ext : primitive::interrupts());

            auto es = es_builder_ ? es_builder_() : std::vector<std::shared_ptr<osi::compiler::logic::instruction_gen>>();
            std::vector<primitive::instruction> insts;
            auto intr_ptr = ext ? ext : std::shared_ptr<primitive::interrupts>(&e.mem().intr(), [](void*){});
            if (!utt_assert.is_true(osi::compiler::logic::importer::compile_to_instructions(es, insts, intr_ptr)))
            {
                return false;
            }

            std::vector<std::string> lines;
            osi::compiler::logic::importer::compile(es, lines, intr_ptr);

            e.load_instructions(std::move(insts));
            e.execute();
            if (e.halt())
            {
                std::cerr << "Execution halted at IP=" << e.reg().ip << ". Instructions:" << std::endl;
                for (size_t i = 0; i < lines.size(); ++i)
                {
                    std::cerr << i << ": " << lines[i] << std::endl;
                }
            }
            utt_assert.is_false(e.halt(), e.halt_error());
            check_result(e);
            return true;
        }
    };
}
