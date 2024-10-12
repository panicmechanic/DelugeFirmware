#pragma once
#include "argon.hpp"
#include "dsp/stereo_sample.h"
#include "util.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace deluge::dsp::util {
using namespace argon;

template <typename T>
constexpr bool is_pow2(T n) {
	return (n & (n - 1)) == 0;
}

template <size_t max_delay>
// requires (is_pow2<size_t>(max_delay))
class StereoDelayLine {

public:
	StereoDelayLine() = default;
	~StereoDelayLine() = default;

	void Init() { Reset(); }

	void Reset() {
		line0_.fill(int32_t(0));
		line1_.fill(int32_t(0));
		write_idx_ = 0;
	}

	inline void Write(StereoSample sample) {
		line0_[write_idx_] = sample.l;
		line1_[write_idx_] = sample.r;
		write_idx_ = (write_idx_ - 1 + max_delay) % max_delay;
	}

	inline const StereoSample Read(size_t delay0, size_t delay1) const {
		Neon64<uint32_t> delay = {delay0, delay1};
		Neon64<uint32_t> idx = (delay + write_idx_) & max_delay;
		return StereoSample{line0_[idx[0]], line1_[idx[1]]};
	}

	inline const StereoSample Read(float delay0, float delay1) const {
		Neon64<float> delay = {delay0, delay1};
		Neon64<int32_t> delay_integral = {static_cast<int32_t>(delay0), static_cast<int32_t>(delay1)};
		Neon64<float> delay_fractional = delay - Neon64<float>{delay_integral[0], delay_integral[1]};
		Neon64<int32_t> idx_a = (write_idx_ + delay_integral) & max_delay;
		Neon64<int32_t> idx_b = (write_idx_ + delay_integral + 1) & max_delay;
		Neon64<float> a = {line0_[idx_a[0]], line1_[idx_a[1]]};
		Neon64<float> b = {line0_[idx_b[0]], line1_[idx_b[1]]};
		Neon64<float> value = a + (b - a) * delay_fractional;
		return StereoSample{static_cast<q31_t>(value[0]), static_cast<q31_t>(value[1])};
	}

	// inline const int32_t ReadHermite(float delay) const {
	// 	MAKE_INTEGRAL_FRACTIONAL(delay)
	// 	int32_t t = (write_idx_ + delay_integral + max_delay);
	// 	const int32_t xm1 = line_[(t - 1) % max_delay];
	// 	const int32_t x0 = line_[(t) % max_delay];
	// 	const int32_t x1 = line_[(t + 1) % max_delay];
	// 	const int32_t x2 = line_[(t + 2) % max_delay];
	// 	const float c = (x1 - xm1) * 0.5f;
	// 	const float v = x0 - x1;
	// 	const float w = c + v;
	// 	const float a = w + v + (x2 - x0) * 0.5f;
	// 	const float b_neg = w + a;
	// 	const float f = delay_fractional;
	// 	return (((a * f) - b_neg) * f + c) * f + x0;
	// }

	inline const StereoSample ReadLagrange3rd(float delay0, float delay1) const {
		Neon64<float> delay = {delay0, delay1};
		Neon64<int32_t> delay_integral = {static_cast<int32_t>(delay0), static_cast<int32_t>(delay1)};
		Neon64<float> delay_integral_float = {static_cast<float>(delay_integral[0]), static_cast<float>(delay_integral[1])};
		Neon64<float> delay_fractional = delay - delay_integral_float;
		Neon64<int32_t> t = (write_idx_ + delay_integral) + max_delay;
		Neon64<int32_t> i0 = t & max_delay;
		Neon64<int32_t> i1 = t + 1 & max_delay;
		Neon64<int32_t> i2 = t + 2 & max_delay;
		Neon64<int32_t> i3 = t + 3 & max_delay;
		Neon64<float> x0 = {static_cast<float>(line0_[i0[0]]), static_cast<float>(line1_[i0[1]])};
		Neon64<float> x1 = {static_cast<float>(line0_[i1[0]]), static_cast<float>(line1_[i1[1]])};
		Neon64<float> x2 = {static_cast<float>(line0_[i2[0]]), static_cast<float>(line1_[i2[1]])};
		Neon64<float> x3 = {static_cast<float>(line0_[i3[0]]), static_cast<float>(line1_[i3[1]])};
		Neon64<float> d0 = delay_fractional - 1.f;
		Neon64<float> d1 = delay_fractional - 2.f;
		Neon64<float> d2 = delay_fractional - 3.f;
		Neon64<float> c0 = -d0 * d1 * d2 / 6.f;
		Neon64<float> c1 = d1 * d2 * 0.5f;
		Neon64<float> c2 = -d0 * d2 * 0.5f;
		Neon64<float> c3 = d0 * d1 / 6.f;
		Neon64<float> f = delay_fractional;
		Neon64<float> value = x0 * c0 + f * (x1 * c1 + x2 * c2 + x3 * c3);
		return StereoSample{static_cast<q31_t>(value[0]), static_cast<q31_t>(value[1])};
	}

private:
	size_t write_idx_;
	std::array<int32_t, max_delay> line0_;
	std::array<int32_t, max_delay> line1_;
};
} // namespace deluge::dsp::util
