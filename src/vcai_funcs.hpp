#pragma once

#include "vcai_data_structs.hpp"
#include "vcai_globals.hpp"

namespace vcai::_intern {

constexpr auto add(i64 &dst, i64 &src1, i64 &src2) noexcept -> void {
    dst = src1 + src2;  // NOLINT C-style cast
}
constexpr auto fadd(f64 &dst, f64 &src1, f64 &src2) noexcept -> void {
    dst = src1 + src2;  // NOLINT C-style cast
}

constexpr auto sub(i64 &dst, i64 &src1, i64 &src2) noexcept -> void {
    dst = src1 - src2;  // NOLINT C-style cast
}
constexpr auto fsub(f64 &dst, f64 &src1, f64 &src2) noexcept -> void {
    dst = src1 - src2;  // NOLINT C-style cast
}

constexpr auto mul(i64 &dst, i64 &src1, i64 &src2) noexcept -> void {
    dst = src1 * src2;  // NOLINT C-style cast
}
constexpr auto fmul(f64 &dst, f64 &src1, f64 &src2) noexcept -> void {
    dst = src1 * src2;  // NOLINT C-style cast
}

constexpr auto div(i64 &dst, i64 &src1, i64 &src2) noexcept -> void {
    dst = src1 / src2;  // NOLINT C-style cast
}
constexpr auto fdiv(f64 &dst, f64 &src1, f64 &src2) noexcept -> void {
    dst = src1 / src2;  // NOLINT C-style cast
}

constexpr auto idiv(i64 &dst, i64 &src1, i64 &src2) noexcept -> void {
    dst = src1 % src2;  // NOLINT C-style cast
}

constexpr auto shr(i64 &dst, i64 &src) noexcept -> void {
    dst >>= src; // NOLINT C-style cast
}

constexpr auto shl(i64 &dst, i64 &src) noexcept -> void {
    dst <<= src; // NOLINT C-style cast
}

constexpr auto v_xor(i64 &dst, i64 &src) noexcept -> void {
    dst ^= src; // NOLINT C-style cast
}

constexpr auto v_and(i64 &dst, i64 &src) noexcept -> void {
    dst &= src; // NOLINT C-style cast
}

constexpr auto v_or(i64 &dst, i64 &src) noexcept -> void {
    dst |= src; // NOLINT C-style cast
}

constexpr auto test(i64 &src) noexcept -> void {
    if (src == 0) ZF = 1; // NOLINT C-style cast
}

constexpr auto cmp(i64 &dst, i64 &src) noexcept -> void {
    if (dst < src) SF = 1; // NOLINT C-style cast
    else if (dst > src) SF = 0; // NOLINT C-style cast
    else if (dst == src) ZF = 1; // NOLINT C-style cast
}

constexpr auto mov(i64 &dst, i64 &src) noexcept -> void {
    dst = src; // NOLINT C-style cast
}

constexpr auto jmp(i64 &dst) noexcept -> void {
    PC = dst;
}

// constexpr auto jl(i64 &dst) noexcept -> void {
//     if (SF != 1) PC = dst;
// }
//
// constexpr auto je(i64 &dst) noexcept -> void {
// }
//
// constexpr auto jg(i64 &dst) noexcept -> void {
// }
//
// constexpr auto jge(i64 &dst) noexcept -> void {
// }
//
// constexpr auto jle(i64 &dst) noexcept -> void {
// }

// constexpr auto call(i64 &dst) noexcept -> void {
// }
//
// constexpr auto ret() noexcept -> void {
// }
//
// constexpr auto push(i64 &dst) noexcept -> void {
// }
//
// constexpr auto pop(i64 &dst) noexcept -> void {
// }

[[nodiscard]] constexpr auto ParseLine();
[[nodiscard]] constexpr auto PerformAction();

}  // namespace vcai::_intern
