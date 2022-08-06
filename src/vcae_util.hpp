#pragma once

static_assert(sizeof(long) == 8,
              "Для i64 нужен 8-байтный целочисленный тип данных!");
using i64 = long;

static_assert(sizeof(double) == 8,
              "Для f64 нужен 8-байтный тип данных c плавающей точкой!");
using f64 = double;

static_assert(sizeof(signed char) == 1,
              "Для byte нужен 1-байтный целочисленный тип данных!");
using byte = signed char;
