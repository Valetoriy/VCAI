#pragma once

/*
__     ______    _    ___
\ \   / / ___|  / \  |_ _|
 \ \ / / |     / _ \  | |
  \ V /| |___ / ___ \ | |
   \_/  \____/_/   \_\___|
Valetoriy Constexpr ASM Interpreter

(c) shadolproff @ github.com/Valetoriy
*/

namespace vcai {

static_assert(sizeof(long) == 8,  // NOLINT magic numbers
              "Для i64 нужен 8-байтный целочисленный тип данных!");
using i64 = long;

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

    [[nodiscard]] constexpr auto front() noexcept -> auto & { return data[0]; }
    [[nodiscard]] constexpr auto front() const noexcept -> auto & {
        return data[0];
    }

    [[nodiscard]] constexpr auto back() noexcept -> auto & {
        return data[m_size - 1];
    }
    [[nodiscard]] constexpr auto back() const noexcept -> auto & {
        return data[m_size - 1];
    }

    [[nodiscard]] constexpr auto is_func() const noexcept -> bool {
        StaticArray funcs{"add",  "sub",  "mul", "div", "mod", "cmp", "mov",
                          "shl",  "shr",  "xor", "and", "or",  "inc", "dec",
                          "jmp",  "jl",   "je",  "jne", "jg",  "jle", "jge",
                          "call", "push", "pop", "ret"};

        for (vcai::size ind{}; ind < funcs.size(); ++ind)
            if (*this == funcs[ind]) return true;

        return false;
    }

    [[nodiscard]] constexpr auto is_i64() const noexcept -> bool {
        if (m_size == 0) return false;

        for (vcai::size ind{}; ind < m_size; ++ind) {
            auto chr{data[ind]};
            if (chr < '0' or chr > '9') {
                if (chr == '-' and ind == 0) continue;
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] constexpr auto to_i64() const noexcept -> i64 {
        if (m_size == 0) return -1;

        i64 ret{}, icount{}, ind{static_cast<i64>(m_size - 1)};
        for (; ind >= 0; --ind) {
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
            auto new_data{new CharType[amount]{}};  // NOLINT no fail check

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

    [[nodiscard]] constexpr auto operator==(const char *other) const noexcept
        -> bool {
        if (m_size != vcai::strlen(other)) return false;

        for (vcai::size ind{}; ind < m_size; ++ind)
            if (data[ind] != other[ind]) return false;

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
        for (i64 ind{}; ind < static_cast<i64>(Size); ++ind)
            if (keys[ind] == key) return ind;

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

template <typename Key1, typename... Keys, typename Val1, typename... Values>
StaticMap(Key1 key1, Keys... keys, Val1 val1, Values... values)
    -> StaticMap<Key1, Val1, sizeof...(Keys) + 1>;

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

class Interpreter {
    StaticArray<i64, 4> IntReg{};
    StaticArray<i64, 4> ArgReg{};
    i64 SP{}, PC{};
    bool ZF{}, SF{};

    static constexpr size STACKSIZE{128};
    StaticArray<i64, STACKSIZE> Stack{};
    DynamicArray<i64> CallStack;
    DynamicMap<String, i64> Labels;

    StaticMap<String, i64 *, 4> StrToIR{
        {String{"r0"}, String{"r1"}, String{"r2"}, String{"r3"}},
        {&IntReg[0], &IntReg[1], &IntReg[2], &IntReg[3]}};

    StaticMap<String, i64 *, 4> StrToAR{
        {String{"a0"}, String{"a1"}, String{"a2"}, String{"a3"}},
        {&ArgReg[0], &ArgReg[1], &ArgReg[2], &ArgReg[3]}};

    // Операции с 3 аргументами
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

    // Операции с 2 аргументами
    static constexpr auto add(i64 &dst, i64 &src1) noexcept -> void {
        dst += src1;
    }

    static constexpr auto sub(i64 &dst, i64 &src1) noexcept -> void {
        dst -= src1;
    }

    static constexpr auto mul(i64 &dst, i64 &src1) noexcept -> void {
        dst *= src1;
    }

    static constexpr auto div(i64 &dst, i64 &src1) noexcept -> void {
        dst /= src1;
    }

    static constexpr auto mod(i64 &dst, i64 &src1) noexcept -> void {
        dst %= src1;
    }

    constexpr auto cmp(i64 &dst, i64 &src) noexcept -> void {
        if (dst < src) {
            SF = true;
            ZF = false;
        } else if (dst > src) {
            SF = false;
            ZF = false;
        } else
            ZF = true;
    }

    static constexpr auto mov(i64 &dst, i64 &src) noexcept -> void {
        dst = src;
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

    // Операции с 1 аргументом
    static constexpr auto inc(i64 &dst) noexcept -> void { ++dst; }

    static constexpr auto dec(i64 &dst) noexcept -> void { --dst; }

    constexpr auto jmp(i64 &dst) noexcept -> void { PC = dst - 1; }

    constexpr auto jl(i64 &dst) noexcept -> void {
        if (SF and !ZF) PC = dst - 1;
    }

    constexpr auto je(i64 &dst) noexcept -> void {
        if (ZF) PC = dst - 1;
    }

    constexpr auto jne(i64 &dst) noexcept -> void {
        if (!ZF) PC = dst - 1;
    }

    constexpr auto jg(i64 &dst) noexcept -> void {
        if (!SF and !ZF) PC = dst - 1;
    }

    constexpr auto jle(i64 &dst) noexcept -> void {
        if (ZF or SF) PC = dst - 1;
    }

    constexpr auto jge(i64 &dst) noexcept -> void {
        if (ZF or !SF) PC = dst - 1;
    }

    constexpr auto call(i64 &dst) noexcept -> void {
        CallStack.push_back(PC);
        PC = dst - 1;
    }

    constexpr auto push(i64 &dst) noexcept -> void {
        Stack[static_cast<size>(SP)] = dst;
        ++SP;
    }

    constexpr auto pop(i64 &dst) noexcept -> void {
        --SP;
        dst = Stack[static_cast<size>(SP)];
    }

    // Операции без аргументов
    constexpr auto ret() noexcept -> void {
        PC = CallStack.back();
        CallStack.pop_back();
    }

    DynamicArray<DynamicArray<String>> prog;

    constexpr auto ToWordArray(const char *txt) noexcept  // NOLINT complexity
        -> void {
        auto len{vcai::strlen(txt)};

        DynamicArray<String> line;
        String word;
        // Проходимся по всем символам текста программы
        for (size ind{}; ind <= len; ++ind) {
            char chr{txt[ind]};
            if (chr != ' ' and chr != '\n' and chr != '\0') {
                word.push_back(chr);
            } else {
                if (not word.is_empty()) {
                    if (word.back() == ':') {
                        // Слово - ярлык
                        if (word == "main:") {  // main: - начало программы
                            CallStack.push_back(0);
                            PC = static_cast<int>(prog.size());
                        }
                        word.pop_back();  // Избавляемся от ':'
                        Labels.push_back(vcai::move(word),
                                         static_cast<i64>(prog.size()));
                    } else
                        line.push_back(vcai::move(word));
                }
                if (chr == '\n' or chr == 0) {
                    if (not line.is_empty() and line[0].front() != '#')
                        // Строка - комментарий
                        prog.push_back(vcai::move(line));
                    else
                        line.clear();
                }
            }
        }
    }

    static constexpr auto call_fn3(const String &func, i64 *dst, i64 *src1,
                                   i64 *src2) noexcept -> void {
        if (func == "add")
            add(*dst, *src1, *src2);
        else if (func == "sub")
            sub(*dst, *src1, *src2);
        else if (func == "mul")
            mul(*dst, *src1, *src2);
        else if (func == "div")
            div(*dst, *src1, *src2);
        else if (func == "mod")
            mod(*dst, *src1, *src2);
        else                  // Синтаксическая ошибка
            *(i64 *)0 = -12;  // NOLINT magic numbers
    }

    constexpr auto call_fn2(const String &func, i64 *dst, i64 *src) noexcept
        -> void {
        if (func == "add")
            add(*dst, *src);
        else if (func == "sub")
            sub(*dst, *src);
        else if (func == "mul")
            mul(*dst, *src);
        else if (func == "div")
            div(*dst, *src);
        else if (func == "mod")
            mod(*dst, *src);
        else if (func == "cmp")
            cmp(*dst, *src);
        else if (func == "mov")
            mov(*dst, *src);
        else if (func == "shl")
            shl(*dst, *src);
        else if (func == "shr")
            shr(*dst, *src);
        else if (func == "xor")
            v_xor(*dst, *src);
        else if (func == "and")
            v_and(*dst, *src);
        else if (func == "or")
            v_or(*dst, *src);
        else                  // Синтаксическая ошибка
            *(i64 *)0 = -12;  // NOLINT magic numbers
    }

    constexpr auto call_fn1(const String &func, i64 *dst) noexcept -> void {
        if (func == "inc")
            inc(*dst);
        else if (func == "dec")
            dec(*dst);
        else if (func == "jmp")
            jmp(*dst);
        else if (func == "jl")
            jl(*dst);
        else if (func == "je")
            je(*dst);
        else if (func == "jne")
            jne(*dst);
        else if (func == "jg")
            jg(*dst);
        else if (func == "jle")
            jle(*dst);
        else if (func == "jge")
            jge(*dst);
        else if (func == "call")
            call(*dst);
        else if (func == "push")
            push(*dst);
        else if (func == "pop")
            pop(*dst);
        else                  // Синтаксическая ошибка
            *(i64 *)0 = -12;  // NOLINT magic numbers
    }

    [[nodiscard]] constexpr auto deref(const String &str) const noexcept
        -> i64 {
        // Первый символ = '&'
        String regname{&str[1]};
        i64 reg{StrToIR.find(regname)};
        if (reg != -1) return *StrToIR[regname];

        reg = StrToAR.find(regname);
        if (reg != -1) return *StrToAR[regname];

        return -1;
    }

    [[nodiscard]] constexpr auto Exec() noexcept -> i64 {  // NOLINT complexity
        const auto prog_size{prog.size()};
        // Для завершения работы интерпретатор должен дойти до конца файла либо
        // опустошить CallStack
        while (PC < static_cast<i64>(prog_size) and !CallStack.is_empty()) {
            auto &line = prog[static_cast<size>(PC)];

            StaticArray<i64 *, 3> lvalues{0, 0, 0};
            StaticArray<i64, 3> rvalues{0, 0, 0};

            const auto line_size{line.size()};
            for (size aind{1}; aind < line_size; ++aind) {
                if (StrToIR.find(line[aind]) != -1)
                    // Слово - регистр r0-3
                    lvalues[aind - 1] = StrToIR[line[aind]];
                else if (StrToAR.find(line[aind]) != -1)
                    // Слово - регистр a0-3
                    lvalues[aind - 1] = StrToAR[line[aind]];
                else if (line[aind].front() == '&') {
                    // Слово - указатель на адрес в стеке
                    auto ind{deref(line[aind])};
                    if (ind != -1 && ind < SP)
                        lvalues[aind - 1] = &Stack[static_cast<size>(ind)];
                } else if (Labels.find(line[aind]) != -1)
                    // Слово - ярлык
                    lvalues[aind - 1] = &Labels[line[aind]];
                else if (line[aind].is_i64()) {
                    // Слово - целочисленная константа
                    rvalues[aind - 1] = line[aind].to_i64();
                    lvalues[aind - 1] = &rvalues[aind - 1];
                }
            }

            // В отличие от регистров, с функциями трюк со StaticMap не работает
            // (по крайнем мере, у меня не вышло)
            String *func{};
            if (line[0].is_func()) func = &line[0];
            if (line_size == 4)
                call_fn3(*func, lvalues[0], lvalues[1], lvalues[2]);
            else if (line_size == 3)
                call_fn2(*func, lvalues[0], lvalues[1]);
            else if (line_size == 2)
                call_fn1(*func, lvalues[0]);
            else if (line_size == 1)
                if (*func == "ret") ret();

            ++PC;
        }  // while

        return IntReg[0];
    }

    // Нельзя создавать вне exec_fn()
    Interpreter() = default;

    friend constexpr auto exec_fn(const char *txt) noexcept -> i64;
};

[[nodiscard]] constexpr auto exec_fn(const char *txt) noexcept -> i64 {
    Interpreter interp{};
    interp.ToWordArray(txt);
    i64 ret{interp.Exec()};

    return ret;
}

}  // namespace vcai
