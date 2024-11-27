#pragma once
#include "definitions_cxx.hpp"
#include "deluge/dsp/stereo_sample.h"
#include <arm_neon_shim.h>
#include <array>
#include <cstdint>

extern const int16_t windowedSincKernel[][17][16];

namespace deluge::dsp {
class Interpolator {
	using buffer_t = std::array<int16x4_t, kInterpolationMaxNumSamples / 4>;

public:
	Interpolator() = default;
	StereoSample interpolate(size_t channels, int32_t whichKernel, uint32_t oscPos);
	StereoSample interpolateLinear(size_t channels, uint32_t oscPos);

	constexpr int16_t& bufferL(size_t idx) {
		// divide by 4, modulo 4
		return interpolation_buffer_l_[idx >> 3][idx & 3];
	}

	constexpr int16_t& bufferR(size_t idx) {
		// divide by 4, modulo 4
		return interpolation_buffer_r_[idx >> 3][idx & 3];
	}

#pragma GCC push_options
#pragma GCC optimize("no-tree-loop-distribute-patterns")
	inline void pushL(int16_t value) {
		// shift interpolation array forwards
		for (int32_t i = kInterpolationMaxNumSamples - 1; i > 0; i--){
			bufferL(i) = bufferL(i - 1);
		}
		interpolation_buffer_l_[0][0] = value;
	}

	inline void pushR(int16_t value) {
		// shift interpolation array forwards
		for (int32_t i = kInterpolationMaxNumSamples - 1; i > 0; i--){
			bufferR(i) = bufferR(i - 1);
		}
		interpolation_buffer_r_[0][0] = value;
	}
#pragma GCC pop_options

	inline void jumpForward(size_t num_samples) {
		for (int32_t i = kInterpolationMaxNumSamples - 1; i >= num_samples; i--) {
			bufferL(i) = bufferL(i - num_samples);
			bufferR(i) = bufferR(i - num_samples);
		}
	}

private:
	buffer_t interpolation_buffer_l_{};
	buffer_t interpolation_buffer_r_{};
};
} // namespace deluge::dsp
