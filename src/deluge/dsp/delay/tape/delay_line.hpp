#pragma once
#include <cstddef>
#include <algorithm>
#include <array>
#include "util.hpp"
#include <argon.hpp>

template <size_t max_delay>
class DualDelayLine {
public:
  DualDelayLine() = default;
  ~DualDelayLine() = default;

  void Init() { Reset(); }

  void Reset() {
	lines_[0].fill(int32_t(0));
	lines_[1].fill(int32_t(0));
	write_ptrs_ = {0, 0};
  }

  inline void Write(const size_t delayline, const int32_t sample) {
    lines_[delayline][write_ptrs_[delayline]] = sample;
    write_ptrs_[delayline] = (write_ptrs_[delayline] - 1 + max_delay) % max_delay;
  }

  inline const int32_t Read(size_t delay) const {
    return line_[(write_ptr_ + delay) % max_delay];
  }

  inline const int32_t Read(float delay) const {
    MAKE_INTEGRAL_FRACTIONAL(delay)
    const int32_t a = line_[(write_ptr_ + delay_integral) % max_delay];
    const int32_t b = line_[(write_ptr_ + delay_integral + 1) % max_delay];
    return a + (b - a) * delay_fractional;
  }

  inline const int32_t ReadHermite(float delay) const {
    MAKE_INTEGRAL_FRACTIONAL(delay)
    int32_t t = (write_ptr_ + delay_integral + max_delay);
    const int32_t xm1 = line_[(t - 1) % max_delay];
    const int32_t x0 = line_[(t) % max_delay];
    const int32_t x1 = line_[(t + 1) % max_delay];
    const int32_t x2 = line_[(t + 2) % max_delay];
    const float c = (x1 - xm1) * 0.5f;
    const float v = x0 - x1;
    const float w = c + v;
    const float a = w + v + (x2 - x0) * 0.5f;
    const float b_neg = w + a;
    const float f = delay_fractional;
    return (((a * f) - b_neg) * f + c) * f + x0;
  }

  inline const int32_t ReadLagrange3rd(float delay) const {
    MAKE_INTEGRAL_FRACTIONAL(delay)
    int32_t t = (write_ptr_ + delay_integral + max_delay);
    const int32_t x0 = line_[t % max_delay];
    const int32_t x1 = line_[t + 1 % max_delay];
    const int32_t x2 = line_[t + 2 % max_delay];
    const int32_t x3 = line_[t + 3 % max_delay];
    const float d0 = delay_fractional - 1.f;
    const float d1 = delay_fractional - 2.f;
    const float d2 = delay_fractional - 3.f;
    const float c0 = -d0 * d1 * d2 / 6.f;
    const float c1 = d1 * d2 * 0.5f;
    const float c2 = -d0 * d2 * 0.5f;
    const float c3 = d0 * d1 / 6.f;
    const float f = delay_fractional;
    return x0 * c0 + f * (x1 * c1 + x2 * c2 + x3 * c3);
  }

private:
  std::array<size_t, 2> write_ptrs_;
  std::array<std::array<int32_t, max_delay>, 2> lines_;
};
