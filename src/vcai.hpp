#pragma once

#include <fmt/core.h>

namespace vcai {

static_assert(sizeof(long) == 8,  // NOLINT magic numbers
              "Для i64 нужен 8-байтный целочисленный тип данных!");
using i64 = long;

// Не очень-то и нужен...
// Просто аналог std::size_t
static_assert(
    sizeof(unsigned long) == 8,  // NOLINT magic numbers
    "Для size нужен 8-байтный целочисленный положительный тип данных!");
using size = unsigned long;

[[nodiscard]] constexpr inline auto strlen(const char *str) noexcept -> size {
    size len{};
    while (str[len] != 0) ++len;  // NOLINT pointer arithmetic

    return len;
}

template <typename T>
[[nodiscard]] constexpr inline auto move(const T &arg) noexcept {
    return (T &&) arg;
}

// Структуры данных
template <typename Contained, vcai::size Size>
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

// Взято с
// https://en.cppreference.com/w/cpp/container/array/deduction_guides
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

    constexpr auto push_back(Contained &&elem) noexcept -> void {
        if (m_size == m_capacity) {
            if (m_capacity > 0)
                reserve(m_capacity * 2);
            else
                reserve(1);
        }
        data[m_size] = vcai::move(elem);  // NOLINT pointer arithmetic
        ++m_size;
    }

    constexpr auto reserve(const vcai::size &amount) noexcept -> void {
        if (amount > m_capacity) {
            auto new_data{new Contained[amount]};

            if (m_capacity > 0) {
                for (vcai::size i{}; i < m_size; ++i)
                    new_data[i] = vcai::move(data[i]);  // NOLINT pointer arithmetic

                delete[](data);
            }
            data = new_data;
            m_capacity = amount;
        }
    }

    constexpr auto clear() noexcept -> void { m_size = 0; }

    constexpr auto is_empty() noexcept -> bool { return m_size == 0; }

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
    constexpr DynamicArray(const DynamicArray &other) noexcept {
        auto len{other.size()};
        reserve(len);
        m_size = len;

        for (vcai::size i{}; i < m_size; ++i)
            data[i] = other[i];  // NOLINT pointer arithmetic
    }
    constexpr DynamicArray(DynamicArray &&other) noexcept
        : data(other.data), m_size(other.size()), m_capacity(other.capacity()) {
        other.data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }
    constexpr auto operator=(const DynamicArray &other) noexcept -> auto & {
        if (&other != this) {
            if (m_capacity > 0) {
                delete[](data);
                m_capacity = 0;
            }

            auto len{other.size()};
            reserve(len);
            m_size = len;

            for (vcai::size i{}; i < m_size; ++i)
                data[i] = other[i];  // NOLINT pointer arithmetic
        }
        return *this;
    }
    constexpr auto operator=(DynamicArray &&other) noexcept -> auto & {
        if (&other != this) {
            if (m_capacity > 0) delete[](data);

            data = other.data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;

            other.data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    constexpr ~DynamicArray() noexcept {
        if (m_capacity > 0) delete[](data);
    }

    [[nodiscard]] constexpr auto begin() noexcept { return data; };
    [[nodiscard]] constexpr auto end() noexcept { return data + m_size; };

    [[nodiscard]] constexpr auto begin() const noexcept { return data; };
    [[nodiscard]] constexpr auto end() const noexcept { return data + m_size; };

    Contained *data{};

   private:
    vcai::size m_size{};
    vcai::size m_capacity{};
};

template <typename Elem1, typename... Elems>
DynamicArray(Elem1, Elems...) -> DynamicArray<Elem1>;

template <typename CharType>
struct BasicString {
    constexpr auto push_back(const CharType &elem) noexcept -> void {
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
            auto new_data{new CharType[amount]};  // NOLINT exc handling

            if (m_capacity > 0) {
                for (vcai::size i{}; i < m_size; ++i)
                    new_data[i] = vcai::move(data[i]);  // NOLINT ...

                delete[](data);
            }
            data = new_data;
            m_capacity = amount;
        }
    }

    constexpr auto clear() noexcept -> void { m_size = 0; }

    constexpr auto is_empty() noexcept -> bool { return m_size == 0; }

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

    // Правило 5
    constexpr BasicString(const BasicString &other) noexcept {
        auto len{other.size()};
        reserve(len);
        m_size = len;

        for (vcai::size i{}; i < m_size; ++i)
            data[i] = other[i];  // NOLINT pointer arithmetic
    }
    constexpr BasicString(BasicString &&other) noexcept
        : data(other.data), m_size(other.size()), m_capacity(other.capacity()) {
        other.data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }
    constexpr auto operator=(const BasicString &other) noexcept -> auto & {
        if (&other != this) {
            if (m_capacity > 0) {
                delete[](data);
                m_capacity = 0;
            }

            auto len{other.size()};
            reserve(len);
            m_size = len;

            for (vcai::size i{}; i < m_size; ++i)
                data[i] = other[i];  // NOLINT pointer arithmetic
        }
        return *this;
    }
    constexpr auto operator=(BasicString &&other) noexcept -> auto & {
        if (&other != this) {
            if (m_capacity > 0) delete[](data);

            data = other.data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;

            other.data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    constexpr ~BasicString() noexcept {
        if (m_capacity > 0) delete[](data);
    }

    [[nodiscard]] constexpr auto begin() noexcept { return data; };
    [[nodiscard]] constexpr auto end() noexcept { return data + m_size; };

    [[nodiscard]] constexpr auto begin() const noexcept { return data; };
    [[nodiscard]] constexpr auto end() const noexcept { return data + m_size; };

    CharType *data{};

   private:
    vcai::size m_size{};
    vcai::size m_capacity{};
};

using String = BasicString<char>;

template <typename Key, typename Value, vcai::size Size>
struct StaticMap {
    [[nodiscard]] constexpr auto size() const noexcept { return Size; }

    [[nodiscard]] constexpr auto find(const Key &key) const noexcept
        -> vcai::size {
        for (vcai::size ind{}; ind < Size; ++ind)
            if (keys[ind] == key) return ind;

        // TODO(shadolproff): добавить универсальный механизм обработки
        // ошибок
        return Size;
    }

    [[nodiscard]] constexpr auto operator[](const Key &key) noexcept -> auto & {
        return values[find(key)];
    }
    [[nodiscard]] constexpr auto operator[](const Key &key) const noexcept
        -> auto & {
        return values[find(key)];
    }

    Key keys[Size];      // NOLINT C-style array
    Value values[Size];  // NOLINT C-style array
};

// Для ASM
struct Interpreter {
    i64 IntReg[8]{};  // NOLINT ...
    i64 ArgReg[8]{};  // NOLINT ...
    i64 SP{}, BP{}, PC{};
    bool ZF{}, SF{};

    i64 Stack[256]{};  // NOLINT ...

    // clang-format off
    StaticMap<const char*, i64*, 8> StrToIR{ // NOLINT ...
        {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"},
        {&IntReg[0], &IntReg[1], &IntReg[2], &IntReg[3],
         &IntReg[4], &IntReg[5], &IntReg[6], &IntReg[7]}, // NOLINT ...
    };

    StaticMap<const char*, i64*, 8> StrToAR{ // NOLINT ...
        {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"},
        {&ArgReg[0], &ArgReg[1], &ArgReg[2], &ArgReg[3],
         &ArgReg[4], &ArgReg[5], &ArgReg[6], &ArgReg[7]}, // NOLINT ...
    };
    // clang-format on

    static constexpr auto add(i64 &dst, i64 &src1, i64 &src2) noexcept -> void {
        dst = src1 + src2;
    }

    static constexpr auto sub(i64 &dst, i64 &src1, i64 &src2) noexcept -> void {
        dst = src1 - src2;
    }

    static constexpr auto mul(i64 &dst, i64 &src1, i64 &src2) noexcept -> void {
        dst = src1 * src2;
    }

    static constexpr auto div(i64 &dst, i64 &src1, i64 &src2) noexcept -> void {
        dst = src1 / src2;
    }

    static constexpr auto mod(i64 &dst, i64 &src1, i64 &src2) noexcept -> void {
        dst = src1 % src2;
    }

    constexpr auto cmp(i64 &dst, i64 &src) noexcept -> void {
        if (dst < src) {
            SF = true;
            ZF = false;
        } else if (dst > src) {
            SF = false;
            ZF = false;
        } else if (dst == src)
            ZF = true;
    }

    static constexpr auto mov(i64 &dst, i64 &src) noexcept -> void {
        dst = src;  // NOLINT C-style cast
    }

    constexpr auto jmp(i64 &dst) noexcept -> void { PC = dst; }

    constexpr auto jl(i64 &dst) noexcept -> void {
        if (SF) PC = dst;
    }

    constexpr auto je(i64 &dst) noexcept -> void {
        if (ZF) PC = dst;
    }

    constexpr auto jne(i64 &dst) noexcept -> void {
        if (!ZF) PC = dst;
    }

    constexpr auto jg(i64 &dst) noexcept -> void {
        if (!SF) PC = dst;
    }

    constexpr auto jle(i64 &dst) noexcept -> void {
        if (ZF or SF) PC = dst;
    }

    constexpr auto jge(i64 &dst) noexcept -> void {
        if (ZF or !SF) PC = dst;
    }

    // constexpr auto call(i64 &dst) noexcept -> void {}
    //
    // constexpr auto ret() noexcept -> void {}
    //
    // constexpr auto push(i64 &dst) noexcept -> void {}
    //
    // constexpr auto pop(i64 &dst) noexcept -> void {}

    static constexpr auto shl(i64 &dst, i64 &src) noexcept -> void {
        dst <<= src;  // NOLINT binary op on int
    }

    static constexpr auto shr(i64 &dst, i64 &src) noexcept -> void {
        dst >>= src;  // NOLINT binary op on int
    }

    static constexpr auto v_xor(i64 &dst, i64 &src) noexcept -> void {
        dst ^= src;  // NOLINT binary op on int
    }

    static constexpr auto v_and(i64 &dst, i64 &src) noexcept -> void {
        dst &= src;  // NOLINT binary op on int
    }

    static constexpr auto v_or(i64 &dst, i64 &src) noexcept -> void {
        dst |= src;  // NOLINT binary op on int
    }

    DynamicArray<DynamicArray<String>> prog;

    constexpr auto Parse(const char *txt) -> void {
        auto len{vcai::strlen(txt)};

        vcai::size line_ind{};
        String word;
        for (vcai::size ind{}; ind < len; ++ind) {
            char chr{txt[ind]};  // NOLINT pointer arithmetic
            if (chr != '\n') {
                word.push_back(chr);
            } else {
                word.clear();
                // if (!prog[line_ind].is_empty()) ++line_ind;
                continue;
            }
        }
    }
    constexpr auto PerformAction() -> void {}

    friend constexpr auto exec_fn(const char *txt) noexcept;
};

[[nodiscard]] constexpr auto exec_fn(const char *txt) noexcept {
    [[maybe_unused]] Interpreter interp{};
    interp.Parse(txt);
    return -12;  // NOLINT magic numbers
}

}  // namespace vcai
