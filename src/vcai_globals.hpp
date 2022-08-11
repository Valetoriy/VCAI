#pragma once

#include "vcai_data_structs.hpp"

namespace vcai::_intern {

constexpr auto STACKSIZE{1024};

static constinit i64 IntReg[8]{};       // NOLINT ...
static constinit i64 ArgReg[8]{};       // NOLINT ...
static constinit i64 SP{}, BP{}, PC{};  // NOLINT ...
static constinit byte ZF{}, SF{};       // NOLINT ...

static constinit byte Stack[STACKSIZE]{};  // NOLINT C-style array

// clang-format off
// NOLINTBEGIN
static constinit StaticMap<const char*, i64*, 8> MapToGPR{
    {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"},
    {&IntReg[0], &IntReg[1], &IntReg[2], &IntReg[3],
     &IntReg[4], &IntReg[5], &IntReg[6], &IntReg[7]},
};

static constinit StaticMap<const char*, i64*, 8> MapToAR{
    {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"},
    {&ArgReg[0], &ArgReg[1], &ArgReg[2], &ArgReg[3],
     &ArgReg[4], &ArgReg[5], &ArgReg[6], &ArgReg[7]},
};
// NOLINTEND
// clang-format on

}  // namespace vcai::_intern
