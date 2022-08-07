#pragma once

#include "vcai_data_structs.hpp"
#include "vcai_globals.hpp"
#include "vcai_util.hpp"

namespace vcai {

template <typename Type>
struct Result {
    char error_msg[128];  // NOLINT magic numbers
    Type value;
};

template <typename Type>
[[nodiscard]] constexpr auto exec_fn(const char *prog) noexcept {
    Result<Type> res;

    return res;
}

}  // namespace vcai
