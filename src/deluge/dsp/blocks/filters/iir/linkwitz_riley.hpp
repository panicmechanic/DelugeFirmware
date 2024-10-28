#pragma once

/** 4th-order L-R Filter */
#include "definitions_cxx.hpp"
#include "dsp/stereo_sample.h"
#include <argon.hpp>
#include <array>
#include <cmath>
#include <cstddef>
#include <numbers>

namespace deluge::dsp::blocks::filters::iir {
class LinkwitzRileyStereo {
public:
	LinkwitzRileyStereo() { setCutoff(cutoff_); }

	constexpr void setCutoff(float freqHz) {
		cutoff_ = freqHz;
		g_ = std::tan(std::numbers::pi_v<float> * cutoff_ / sample_rate);
		h_ = (1.0f / (1.0f + R2 * g_ + g_ * g_));
	}

	void init() {
		setCutoff(cutoff_);
		state.fill(0.f);
	}

	// Two inputs, two outputs
	float block(float& s, float x) {
		float t = g_ * x;
		float y = t + s;
		s = t + y;
		return y;
	}

	argon::Neon64<float> block_neon(argon::Neon64<float>& s, argon::Neon64<float> x) {
		using namespace argon;
		Neon64<float> g_vec{g_};
		s.MultiplyAdd(g_vec, x); // s += (g * x)
		Neon64<float> y = s;     // y = s
		s.MultiplyAdd(g_vec, x); // s += (g * x)
		return y;
	}

	inline void processSample(StereoFloatSample x, StereoFloatSample& outputLow, StereoFloatSample& outputHigh) {
		using namespace argon;
		auto& [s1, s2, s3, s4] = state;
		Neon64<float> x_vec{&x.l};

		Neon64<float> yH = (x_vec - (R2 + g_) * s1 - s2) * h_;

		Neon64<float> yB = block_neon(s1, yH);
		Neon64<float> yL = block_neon(s2, yB);

		Neon64<float> yH2 = (yL - (R2 + g_) * s3 - s4) * h_;

		Neon64<float> yB2 = block_neon(s2, yH2);
		Neon64<float> yL2 = block_neon(s2, yB2);

		yL2.Store(&outputLow.l);

		Neon64<float> high = yL - (R2 * yB) + yH - yL2;
		high.Store(&outputHigh.l);
	}

private:
	float g_, h_;
	static constexpr float sample_rate = kSampleRate;
	static constexpr auto R2 = std::numbers::sqrt2_v<float>;
	std::array<argon::Neon64<float>, 4> state;
	float cutoff_ = 2000.0;
};
}
