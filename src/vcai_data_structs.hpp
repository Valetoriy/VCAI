#pragma once

#include "vcai_util.hpp"

namespace vcai {

template <typename Contained, size Size>
struct StaticArray {
    [[nodiscard]] constexpr auto size() const noexcept { return Size; }

    [[nodiscard]] constexpr auto operator[](const vcai::size &ind) noexcept
        -> auto & {
        return data[ind];
    }
    [[nodiscard]] constexpr auto operator[](
        const vcai::size &ind) const noexcept -> auto & {
        return data[ind];
    }

    [[nodiscard]] constexpr auto begin() noexcept { return data; };
    [[nodiscard]] constexpr auto end() noexcept { return data + Size; };

    [[nodiscard]] constexpr auto begin() const noexcept { return data; };
    [[nodiscard]] constexpr auto end() const noexcept { return data + Size; };

    Contained data[Size];  // NOLINT C-style arrays
};

// Взято с https://en.cppreference.com/w/cpp/container/array/deduction_guides
template <typename Elem1, typename... Elems>
StaticArray(Elem1, Elems...) -> StaticArray<Elem1, sizeof...(Elems) + 1>;

template <typename Contained>
struct DynamicArray {
    constexpr auto push_back(const Contained &elem) noexcept -> void {
        if (size == capacity) {
            if (capacity > 0)
                reserve(capacity * 2);
            else
                reserve(1);
        }
        data[size] = elem;  // NOLINT pointer arithmetic
        ++size;
    }

    constexpr auto reserve(const vcai::size &amount) noexcept -> void {
        if (amount > capacity) {
            auto new_data{new Contained[amount]};

            if (capacity > 0) {
                for (vcai::size i{}; i < size; ++i)
                    new_data[i] = data[i];  // NOLINT pointer arithmetic

                delete[](data);
            }
            data = new_data;
            capacity = amount;
        }
    }

    [[nodiscard]] constexpr auto operator[](const vcai::size &ind) noexcept
        -> auto & {
        return data[ind];  // NOLINT pointer arithmetic
    }
    [[nodiscard]] constexpr auto operator[](
        const vcai::size &ind) const noexcept -> auto & {
        return data[ind];  // NOLINT pointer arithmetic
    }

    [[nodiscard]] constexpr DynamicArray() noexcept = default;

    template <typename Elem1, typename... Elems>
    [[nodiscard]] constexpr explicit DynamicArray(
        const Elem1 &elem, const Elems &...elems) noexcept {
        constexpr vcai::size amount{sizeof...(elems) + 1};
        reserve(sizeof...(elems) + 1);
        size = amount;

        data[0] = elem;  // NOLINT pointer arithmetic
        if constexpr (amount > 1) {
            vcai::size ind{1};
            for (const auto &e : {elems...}) {  // NOLINT short name
                data[ind] = e;                  // NOLINT pointer arithmetic
                ++ind;
            }
        }
    }

    // Правило 5
    constexpr DynamicArray(const DynamicArray &other) noexcept = delete;
    constexpr DynamicArray(DynamicArray &&other) noexcept = delete;
    constexpr auto operator=(const DynamicArray &other) noexcept = delete;
    constexpr auto operator=(DynamicArray &&other) noexcept = delete;

    constexpr ~DynamicArray() noexcept {
        if (capacity > 0) delete[](data);
    }

    [[nodiscard]] constexpr auto begin() noexcept { return data; };
    [[nodiscard]] constexpr auto end() noexcept { return data + size; };

    [[nodiscard]] constexpr auto begin() const noexcept { return data; };
    [[nodiscard]] constexpr auto end() const noexcept { return data + size; };

    Contained *data;
    vcai::size size{};
    vcai::size capacity{};
};

template <typename Elem1, typename... Elems>
DynamicArray(Elem1, Elems...) -> DynamicArray<Elem1>;

}  // namespace vcai
