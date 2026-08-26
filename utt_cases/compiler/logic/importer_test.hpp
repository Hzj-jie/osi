#pragma once
#include "../../../compiler/logic/importer.hpp"
#include "../../../primitive/simulator.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace logic_test
{
    class importer_test : public icase
    {
    public:
        bool run() override
        {
            using namespace osi::compiler::logic;

            // import_empty
            {
                std::vector<std::shared_ptr<instruction_gen>> es;
                importer imp;
                utt_assert.is_false(imp.import("", es));
            }

            // importable
            {
                std::vector<std::string> importable_cases = {
                    "define v type*",
                    "type int 4\ndefine v int\ndefine s int\nsizeof s v"
                };

                for (const auto& code : importable_cases)
                {
                    std::vector<std::shared_ptr<instruction_gen>> es;
                    importer imp;
                    utt_assert.is_true(imp.import(code, es));
                    utt_assert.is_false(es.empty());
                }
            }

            // not_importable
            {
                std::vector<std::string> not_importable_cases = {
                    "unknown",
                    "define"
                };

                for (const auto& code : not_importable_cases)
                {
                    std::vector<std::shared_ptr<instruction_gen>> es;
                    importer imp;
                    utt_assert.is_false(imp.import(code, es));
                }
            }

            // execute_cases
            {
                std::string code = 
                    "type int 4\n"
                    "type bool 1\n"
                    "define upper_bound int\n"
                    "copy_const upper_bound i1000\n"
                    "define 1 int\n"
                    "copy_const 1 i1\n"
                    "define i type*\n"
                    "define result type*\n"
                    "copy_const i i0\n"
                    "define continue bool\n"
                    "less continue i upper_bound\n"
                    "while_then continue { \n"
                    "add i i 1\n"
                    "add result result i\n"
                    "less continue i upper_bound\n"
                    "}\n";

                primitive::simulator sim;
                importer imp;
                utt_assert.is_true(imp.import(code, sim));
                sim.execute();
                utt_assert.is_false(sim.halt(), sim.halt_error());

                utt_assert.equal(sim.access_as_uint32(primitive::data_ref::abs(0)), static_cast<uint32_t>(1000));
                utt_assert.equal(sim.access_as_uint32(primitive::data_ref::abs(1)), static_cast<uint32_t>(1));
                utt_assert.equal(sim.access_as_uint32(primitive::data_ref::abs(2)), static_cast<uint32_t>(1000));
                utt_assert.equal(sim.access_as_uint32(primitive::data_ref::abs(3)), static_cast<uint32_t>(500500));
                utt_assert.equal(sim.access_as_bool(primitive::data_ref::abs(4)), false);
            }

            return true;
        }

        DEFINE_CASE(importer_test);
    };

    REGISTER_CASE(importer_test);
}
