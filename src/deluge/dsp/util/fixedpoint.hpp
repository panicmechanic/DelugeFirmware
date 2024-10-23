#pragma once
#include "util/fixedpoint.h"
#include <argon.hpp>

namespace deluge::dsp::util::fixedpoint {

///@brief Clips Q63 to Q31 values.
constexpr q31_t clip_q63(q63_t x) {
    if ((q31_t) (x >> 32) != ((q31_t) x >> 31)) {
        return ((0x7FFFFFFF ^ ((q31_t) (x >> 63))));
    } else {
        return (q31_t)x;
    }
}

constexpr float toFloat(q31_t x) {
    return (float) x / 2147483648.0f;
}

constexpr q31_t fromFloat(float x){
    return clip_q63((q63_t) (x * 2147483648.0f));
}

void fromFloat(const std::span<float> input, std::span<q31_t> output) {
  const size_t size = input.size();
  const size_t blocks_end = size & ~0x3;

  size_t i;
  for (i = 0; i < blocks_end; i += 4) {
    auto inV = argon::Neon128<float>::Load(&input[i]);
    auto outV = inV.Convert<int32_t>(31);
    outV.Store(&output[i]);
  }

  for (; i < size; ++i) {
    output[i] = fromFloat(input[i]);
  }
}

void toFloat(const std::span<q31_t> input, std::span<float> output) {
  const size_t size = input.size();
  const size_t blocks_end = size & ~0x3;

  size_t i;
  for (i = 0; i < blocks_end; i += 4) {
    auto inV = argon::Neon128<int32_t>::Load(&input[i]);
    auto outV = inV.Convert<float>(31);
    outV.Store(&output[i]);
  }

  for (; i < size; ++i) {
    output[i] = toFloat(input[i]);
  }
}
}
