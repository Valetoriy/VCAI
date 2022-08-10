#pragma once

#include <fmt/core.h>

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
    auto len{vcai::strlen(prog)};
    vcai::String line;

    for (vcai::size ind{}; ind < len; ++ind) {
        char chr{prog[ind]};
        if (chr != '\n') {
            line.push_back(chr);
        } else {
            fmt::print("{}\n", line);
            line.clear();
            continue;
        }
    }

    Result<Type> res;
    return res;
}

}  // namespace vcai
