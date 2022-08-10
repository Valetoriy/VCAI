#pragma once

#include "vcai_data_structs.hpp"

namespace vcai::_intern {
//
// constexpr auto IDSIZE{8};
// constexpr auto STACKSIZE{1024};
//
// struct Identifier {
//     byte arr[IDSIZE];  // NOLINT C-style array
// };
//
// static constinit Identifier GenPurpReg[8]{};   // NOLINT ...
// static constinit Identifier ArgumentReg[8]{};  // NOLINT ...
// static constinit Identifier SP{}, BP{}, PC{};  // NOLINT ...
//
// static constinit byte Stack[STACKSIZE];  // NOLINT C-style array
//
// // NOLINTBEGIN
// static constinit StaticMap<const char*, Identifier*, 8> MapToGPR{
//     {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"},
//     {&GenPurpReg[0], &GenPurpReg[1], &GenPurpReg[2], &GenPurpReg[3],
//      &GenPurpReg[4], &GenPurpReg[5], &GenPurpReg[6], &GenPurpReg[7]},
// };
// // NOLINTEND
//
// // NOLINTBEGIN
// static constinit StaticMap<const char*, Identifier*, 8> MapToAR{
//     {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"},
//     {&ArgumentReg[0], &ArgumentReg[1], &ArgumentReg[2], &ArgumentReg[3],
//      &ArgumentReg[4], &ArgumentReg[5], &ArgumentReg[6], &ArgumentReg[7]},
// };
// // NOLINTEND
//
}  // namespace vcai::_intern
