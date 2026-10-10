#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../../../app_info/assert.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../logic/logic_writer.hpp"
#include "../scope.hpp"
#include "../b3style_forward.hpp"

#define DIALECT b3style_compiler
#define WRITER logic_writer
#include "../../b2style/nodes/namespaces.inc"

