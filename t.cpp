#include <fmt/format.h>

#include "vcae_util.hpp"

constexpr auto prog1{R"(
mov r0, a0
mov r1, a1
add r0, r1

ret
)"};

constexpr auto prog2{R"(
str "Hello world\n"
mov r0, str

ret
)"};

int main() {
    // res1 это i64
    auto res1{vcae::exec_fn<i64>(prog1, 12, 42)};
    fmt::print("{}\n", res1);

    // res2 это const char *
    auto res2{vcae::exec_fn<const char*>(prog2)};
    fmt::print("{}\n", res2);
    return 0;
}
