#pragma once
#include <cstdint>

#define MAKE_INTEGRAL_FRACTIONAL(x)               \
  int32_t x##_integral = static_cast<int32_t>(x); \
  float x##_fractional = x - static_cast<float>(x##_integral);

