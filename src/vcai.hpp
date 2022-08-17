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

[[nodiscard]] constexpr auto strlen(const char *str) noexcept -> size {
    size len{};
    while (str[len] != 0) ++len;

    return len;
}

template <typename T>
[[nodiscard]] constexpr auto move(const T &arg) noexcept {
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

    // TODO(shadolproff): протестировать std::array в CE
    Contained data[Size];
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
        data[m_size] = elem;
        ++m_size;
    }

    constexpr auto push_back(Contained &&elem) noexcept -> void {
        if (m_size == m_capacity) {
            if (m_capacity > 0)
                reserve(m_capacity * 2);
            else
                reserve(1);
        }
        data[m_size] = vcai::move(elem);
        ++m_size;
    }

    constexpr auto pop_back() noexcept -> void {
        if (m_size > 0) --m_size;
    }

    constexpr auto back() noexcept -> auto & { return data[m_size - 1]; }

    constexpr auto reserve(const vcai::size &amount) noexcept -> void {
        if (amount > m_capacity) {
            auto new_data{new Contained[amount]};  // NOLINT no fail check

            if (m_capacity > 0) {
                for (vcai::size i{}; i < m_size; ++i)
                    new_data[i] = vcai::move(data[i]);

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
        return data[ind];
    }
    [[nodiscard]] constexpr auto operator[](
        const vcai::size &ind) const noexcept -> auto & {
        return data[ind];
    }

    [[nodiscard]] constexpr DynamicArray() noexcept = default;

    template <typename Elem1, typename... Elems>
    [[nodiscard]] constexpr explicit DynamicArray(
        const Elem1 &elem, const Elems &...elems) noexcept {
        constexpr vcai::size amount{sizeof...(elems) + 1};
        reserve(amount);
        m_size = amount;

        data[0] = elem;
        if constexpr (amount > 1) {
            vcai::size ind{1};
            for (const auto &e : {elems...}) {  // NOLINT short name
                data[ind] = e;
                ++ind;
            }
        }
    }

    // Правило 5
    constexpr DynamicArray(const DynamicArray &other) noexcept {
        auto len{other.size()};
        reserve(len);
        m_size = len;

        for (vcai::size i{}; i < m_size; ++i) data[i] = other[i];
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

            for (vcai::size i{}; i < m_size; ++i) data[i] = other[i];
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
        data[m_size] = elem;
        ++m_size;
    }

    constexpr auto pop_back() noexcept -> void {
        if (m_size > 0) --m_size;
    }

    constexpr auto front() noexcept -> auto & { return data[0]; }

    constexpr auto back() noexcept -> auto & { return data[m_size - 1]; }

    [[nodiscard]] constexpr auto to_i64() const noexcept -> i64 {
        if (m_size == 0) return -1;

        i64 ret{}, icount{};
        for (i64 ind{static_cast<i64>(m_size - 1)}; ind >= 0; --ind) {
            auto chr{data[ind]};
            if (chr >= '0' and chr <= '9') {
                // 48 - ноль в ASCII
                chr -= 48;  // NOLINT magic numbers

                // Аналог std::pow()
                i64 pow{1};
                for (i64 exp{}; exp < icount; ++exp) pow *= 10;  // NOLINT ...
                ret += chr * pow;
                ++icount;
            }
        }
        if (data[0] == '-') ret *= -1;

        return ret;
    }

    constexpr auto reserve(const vcai::size &amount) noexcept -> void {
        if (amount > m_capacity) {
            auto new_data{new CharType[amount]};  // NOLINT no fail check

            if (m_capacity > 0) {
                for (vcai::size i{}; i < m_size; ++i)
                    new_data[i] = vcai::move(data[i]);

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
        return data[ind];
    }
    [[nodiscard]] constexpr auto operator[](
        const vcai::size &ind) const noexcept -> auto & {
        return data[ind];
    }

    [[nodiscard]] constexpr auto operator==(
        const BasicString &other) const noexcept -> bool {
        if (m_size != other.m_size) return false;

        for (vcai::size ind{}; ind < m_size; ++ind)
            if (data[ind] != other.data[ind]) return false;

        return true;
    }

    [[nodiscard]] constexpr BasicString() noexcept = default;

    [[nodiscard]] constexpr explicit BasicString(const char *str) noexcept {
        auto len{vcai::strlen(str)};
        reserve(len);
        m_size = len;

        for (vcai::size i{}; i < m_size; ++i) data[i] = str[i];
    }

    // Правило 5
    constexpr BasicString(const BasicString &other) noexcept {
        auto len{other.size()};
        reserve(len);
        m_size = len;

        for (vcai::size i{}; i < m_size; ++i) data[i] = other[i];
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

            for (vcai::size i{}; i < m_size; ++i) data[i] = other[i];
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

    [[nodiscard]] constexpr auto find(const Key &key) const noexcept -> i64 {
        for (i64 ind{}; ind < Size; ++ind)
            if (keys[ind] == key) return ind;

        // TODO(shadolproff): добавить универсальный механизм обработки
        // ошибок
        return -1;
    }

    [[nodiscard]] constexpr auto operator[](const Key &key) noexcept -> auto & {
        return values[find(key)];
    }
    [[nodiscard]] constexpr auto operator[](const Key &key) const noexcept
        -> auto & {
        return values[find(key)];
    }

    Key keys[Size];
    Value values[Size];
};

template <typename Key, typename Value>
struct DynamicMap {
    constexpr auto push_back(Key &&key, Value &&val) noexcept -> void {
        if (m_size == m_capacity) {
            if (m_capacity > 0)
                reserve(m_capacity * 2);
            else
                reserve(1);
        }
        keys[m_size] = vcai::move(key);
        values[m_size] = vcai::move(val);
        ++m_size;
    }

    constexpr auto reserve(const vcai::size &amount) noexcept -> void {
        if (amount > m_capacity) {
            auto new_keys{new Key[amount]};      // NOLINT no fail check
            auto new_values{new Value[amount]};  // NOLINT no fail check

            if (m_capacity > 0) {
                for (vcai::size i{}; i < m_size; ++i) {
                    new_keys[i] = vcai::move(keys[i]);
                    new_values[i] = vcai::move(values[i]);
                }

                delete[](keys);
                delete[](values);
            }
            keys = new_keys;
            values = new_values;
            m_capacity = amount;
        }
    }

    [[nodiscard]] constexpr auto size() const noexcept { return m_size; }
    [[nodiscard]] constexpr auto capacity() const noexcept {
        return m_capacity;
    }

    [[nodiscard]] constexpr auto find(const Key &key) const noexcept -> i64 {
        for (vcai::size ind{}; ind < m_size; ++ind)
            if (keys[ind] == key) return static_cast<i64>(ind);

        return -1;
    }

    [[nodiscard]] constexpr auto operator[](const Key &key) noexcept -> auto & {
        return values[find(key)];
    }
    [[nodiscard]] constexpr auto operator[](const Key &key) const noexcept
        -> auto & {
        return values[find(key)];
    }

    [[nodiscard]] constexpr DynamicMap() noexcept = default;

    // Правило 5
    constexpr DynamicMap(const DynamicMap &other) noexcept = delete;
    constexpr DynamicMap(DynamicMap &&other) noexcept = delete;
    constexpr auto operator=(const DynamicMap &other) noexcept = delete;
    constexpr auto operator=(DynamicMap &&other) noexcept = delete;

    constexpr ~DynamicMap() noexcept {
        if (m_capacity > 0) {
            delete[](keys);
            delete[](values);
        }
    }

    Key *keys{};
    Value *values{};

   private:
    vcai::size m_size{};
    vcai::size m_capacity{};
};

// Для ASM
struct Interpreter {
    i64 IntReg[4]{};
    i64 ArgReg[4]{};
    i64 SP{}, BP{}, PC{};
    bool ZF{}, SF{};

    static constexpr vcai::size STACKSIZE{128};
    i64 Stack[STACKSIZE]{};
    DynamicArray<i64> CallStack;
    DynamicMap<String, i64> Labels;

    StaticMap<const char *, i64 *, 4> StrToIR{
        {"r0", "r1", "r2", "r3"},
        {&IntReg[0], &IntReg[1], &IntReg[2], &IntReg[3]}};

    StaticMap<const char *, i64 *, 4> StrToAR{
        {"a0", "a1", "a2", "a3"},
        {&ArgReg[0], &ArgReg[1], &ArgReg[2], &ArgReg[3]}};

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
        dst = src;
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

    constexpr auto call(i64 &dst) noexcept -> void {
        CallStack.push_back(PC + 1);
        PC = dst;
    }

    constexpr auto ret() noexcept -> void {
        PC = CallStack.back();
        CallStack.pop_back();
    }

    constexpr auto push(i64 &dst) noexcept -> void {
        Stack[SP] = dst;
        ++SP;
    }

    constexpr auto pop(i64 &dst) noexcept -> void {
        --SP;
        dst = Stack[SP];
    }

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

    constexpr auto ToWordArray(const char *txt) -> void {
        auto len{vcai::strlen(txt)};

        DynamicArray<String> line;
        String word;
        for (vcai::size ind{}; ind <= len; ++ind) {
            char chr{txt[ind]};
            if (chr != ' ' and chr != '\n' and chr != '\0') {
                word.push_back(chr);
            } else {
                if (not word.is_empty()) {
                    if (word.back() == ':')
                        Labels.push_back(vcai::move(word),
                                         static_cast<i64>(prog.size()));
                    else
                        line.push_back(vcai::move(word));
                }
                if (chr == '\n' or chr == '\0') {
                    if (not line.is_empty() and line[0].front() != '#')
                        prog.push_back(vcai::move(line));
                    else
                        line.clear();
                }
            }
        }
    }

    constexpr auto Exec() -> void {}

    friend constexpr auto exec_fn(const char *txt) noexcept;
};

[[nodiscard]] constexpr auto exec_fn(const char *txt) noexcept {
    [[maybe_unused]] Interpreter interp{};
    interp.ToWordArray(txt);
    interp.Exec();

    return interp.IntReg[0];
}

}  // namespace vcai
