#pragma once

namespace vcai {

static_assert(sizeof(long) == 8,  // NOLINT magic numbers
              "Для i64 нужен 8-байтный целочисленный тип данных!");
using i64 = long;

static_assert(sizeof(double) == 8,  // NOLINT magic numbers
              "Для f64 нужен 8-байтный тип данных c плавающей точкой!");
using f64 = double;

static_assert(sizeof(signed char) == 1,  // NOLINT magic numbers
              "Для byte нужен 1-байтный целочисленный тип данных!");
using byte = signed char;

// Не очень-то и нужен...
// Просто аналог std::size_t
static_assert(
    sizeof(unsigned long) == 8,  // NOLINT magic numbers
    "Для size нужен 8-байтный целочисленный положительный тип данных!");
using size = unsigned long;

}  // namespace vcai
