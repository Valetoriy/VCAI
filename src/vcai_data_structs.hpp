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
        if (m_size == m_capacity) {
            if (m_capacity > 0)
                reserve(m_capacity * 2);
            else
                reserve(1);
        }
        data[m_size] = elem;  // NOLINT pointer arithmetic
        ++m_size;
    }

    constexpr auto reserve(const vcai::size &amount) noexcept -> void {
        if (amount > m_capacity) {
            auto new_data{new Contained[amount]};

            if (m_capacity > 0) {
                for (vcai::size i{}; i < m_size; ++i)
                    new_data[i] = data[i];  // NOLINT pointer arithmetic

                delete[](data);
            }
            data = new_data;
            m_capacity = amount;
        }
    }

    [[nodiscard]] constexpr auto size() const noexcept { return m_size; }
    [[nodiscard]] constexpr auto capacity() const noexcept {
        return m_capacity;
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
        reserve(amount);
        m_size = amount;

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
        if (m_capacity > 0) delete[](data);
    }

    [[nodiscard]] constexpr auto begin() noexcept { return data; };
    [[nodiscard]] constexpr auto end() noexcept { return data + m_size; };

    [[nodiscard]] constexpr auto begin() const noexcept { return data; };
    [[nodiscard]] constexpr auto end() const noexcept { return data + m_size; };

    Contained *data;

   private:
    vcai::size m_size{};
    vcai::size m_capacity{};
};

template <typename Elem1, typename... Elems>
DynamicArray(Elem1, Elems...) -> DynamicArray<Elem1>;

template <typename CharType>
struct BasicString {
    constexpr auto reserve(const vcai::size &amount) noexcept -> void {
        if (amount > m_capacity) {
            auto new_data{new CharType[amount]};

            if (m_capacity > 0) {
                for (vcai::size i{}; i < m_size; ++i)
                    new_data[i] = data[i];  // NOLINT pointer arithmetic

                delete[](data);
            }
            data = new_data;
            m_capacity = amount;
        }
    }

    [[nodiscard]] constexpr auto size() const noexcept { return m_size; }
    [[nodiscard]] constexpr auto capacity() const noexcept {
        return m_capacity;
    }

    [[nodiscard]] constexpr auto operator[](const vcai::size &ind) noexcept
        -> auto & {
        return data[ind];  // NOLINT pointer arithmetic
    }
    [[nodiscard]] constexpr auto operator[](
        const vcai::size &ind) const noexcept -> auto & {
        return data[ind];  // NOLINT pointer arithmetic
    }

    [[nodiscard]] constexpr BasicString() noexcept = default;

    [[nodiscard]] constexpr explicit BasicString(const char *str) noexcept {
        auto len{vcai::strlen(str)};
        reserve(len);
        m_size = len;

        for (vcai::size i{}; i < m_size; ++i)
            data[i] = str[i];  // NOLINT pointer arithmetic
    }

    constexpr ~BasicString() noexcept {
        if (m_capacity > 0) delete[](data);
    }

    [[nodiscard]] constexpr auto begin() noexcept { return data; };
    [[nodiscard]] constexpr auto end() noexcept { return data + m_size; };

    [[nodiscard]] constexpr auto begin() const noexcept { return data; };
    [[nodiscard]] constexpr auto end() const noexcept { return data + m_size; };

    CharType *data;

   private:
    vcai::size m_size{};
    vcai::size m_capacity{};
};

using String = BasicString<char>;

}  // namespace vcai
