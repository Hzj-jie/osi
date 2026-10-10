#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../../../app_info/assert.hpp"
#include "../../function_signature.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "../../logic/scope.hpp"
#include "../scope.hpp"

#define DIALECT b3style_compiler
#include "../../bstyle/nodes/logic_name.inc"
#undef DIALECT
