#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../../interpreter/primitive/data_block.hpp"
#include "../../../math/big_int.hpp"
#include "../../../math/big_dec.hpp"
#include "../../../utils/strutils.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../code_gen/includes.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "../../logic/scope.hpp"
#include "../scope.hpp"
#include "../bstyle_forward.hpp"

#define DIALECT bstyle_compiler
#include "raw_value.inc"
#undef DIALECT
