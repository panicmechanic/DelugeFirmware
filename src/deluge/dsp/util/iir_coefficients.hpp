#pragma once
#include "fixedpoint.hpp"
#include "util/fixedpoint.h"
#include "util/unit_convernsions.h"
#include <cfloat>
#include <cmath>
#include <numbers>

namespace deluge::dsp::util::iir {
template <typename T>
struct Coefficients {
	T b0;
	T b1;
	T b2;
	T a0;
	T a1;
	T a2;

	Coefficients<q31_t> asQ31() {
		return Coefficients<q31_t> {
			.b0 = fixedpoint::fromFloat(b0),
			.b1 = fixedpoint::fromFloat(b1),
			.b2 = fixedpoint::fromFloat(b2),
			.a0 = fixedpoint::fromFloat(a0),
			.a1 = fixedpoint::fromFloat(a1),
			.a2 = fixedpoint::fromFloat(a2),
		};
	}
};

// Based on Fig. 27 of https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html and JUCE library
Coefficients<float> peakEQ(float sample_rate, float frequency, float Q, float gainFactor) {
	constexpr auto minDb = -300.f;
	const auto A = std::sqrt(std::max(gainFactor, deluge::util::DecibelsToGain(minDb, minDb - 1.f)));
	const auto omega = (2.f * std::numbers::pi_v<float> * std::max(frequency, 2.0f)) / sample_rate; // Fig. 6
	const auto alpha = 0.5f * std::sin(omega) / Q;                                                  // Fig. 7
	const auto c2 = -2.0f * std::cos(omega);
	const auto alphaTimesA = alpha * A;
	const auto alphaOverA = alpha / A;

	return Coefficients<float>{1.0f + alphaTimesA, c2, 1.0f - alphaTimesA, 1.0f + alphaOverA, c2, 1.0f - alphaOverA};
}
} // namespace deluge::dsp::util::iir
