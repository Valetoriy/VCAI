#pragma once

#include "vcai_util.hpp"

namespace vcai {

template <typename Contained, size Size>
struct StaticArray {
    [[nodiscard]] constexpr auto size() const noexcept { return Size; }

    [[nodiscard]] constexpr auto operator[](
        const vcai::size &s)  // NOLINT short name
        -> auto & {
        return data[s];
    }
    [[nodiscard]] constexpr auto operator[](
        const vcai::size &s) const  // NOLINT short name
        -> auto & {
        return data[s];
    }

    [[nodiscard]] constexpr auto begin() noexcept { return data; };
    [[nodiscard]] constexpr auto end() noexcept { return data + Size; };

    [[nodiscard]] constexpr auto begin() const noexcept { return data; };
    [[nodiscard]] constexpr auto end() const noexcept { return data + Size; };
    Contained data[Size];  // NOLINT C-style arrays
};

// Взято с https://en.cppreference.com/w/cpp/container/array/deduction_guides
template <typename Cont1, typename... Cont2>
StaticArray(Cont1, Cont2...) -> StaticArray<Cont1, sizeof...(Cont2) + 1>;

}  // namespace vcai
