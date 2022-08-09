#pragma once

namespace vcai {

static_assert(sizeof(long) == 8,  // NOLINT magic numbers
              "Для i64 нужен 8-байтный целочисленный тип данных!");
using i64 = long;

static_assert(sizeof(double) == 8,  // NOLINT magic numbers
              "Для f64 нужен 8-байтный тип данных c плавающей точкой!");
using f64 = double;

static_assert(sizeof(char) == 1,  // NOLINT magic numbers
              "Для byte нужен 1-байтный целочисленный положильный тип данных!");
using byte = char;

// Не очень-то и нужен...
// Просто аналог std::size_t
static_assert(
    sizeof(unsigned long) == 8,  // NOLINT magic numbers
    "Для size нужен 8-байтный целочисленный положительный тип данных!");
using size = unsigned long;

[[nodiscard]] constexpr inline auto strlen(const char *str) noexcept -> size {
    size len{}, ind{};
    while (str[ind] != 0) {  // NOLINT pointer arithmetic
        ++len;
        ++ind;
    }

    return len;
}
}  // namespace vcai
