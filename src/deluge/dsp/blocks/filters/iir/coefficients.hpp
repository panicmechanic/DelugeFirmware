#pragma once
#include "dsp/util/fixedpoint.hpp"
#include "util/fixedpoint.h"
#include "util/unit_convernsions.h"
#include <cfloat>
#include <cmath>
#include <numbers>

namespace deluge::dsp::blocks::filters::iir {
template <typename T>
struct Coefficients {
	T b0;
	T b1;
	T b2;
	T a0;
	T a1;
	T a2;

	/// @brief Normalize so that a0 is 1
	/// @return Normalized coefficients
	Coefficients<float> normalizeA0() {
		static_assert(std::is_same_v<T, float>);
		return Coefficients<float>{
		    .b0 = b0 / a0,
		    .b1 = b1 / a0,
		    .b2 = b2 / a0,
		    .a0 = a0 / a0,
		    .a1 = a1 / a0,
		    .a2 = a2 / a0,
		};
	}

	std::pair<Coefficients<q31_t>, size_t> toQ31() {
		float max_value = std::max({b0, b1, b2, a0, a1, a2});
		size_t scale;
		for (scale = 0; scale < 31; scale++) {
			if (max_value / std::exp2f(scale) < 1.f) {
				break;
			}
		}

		const float scale_factor = 1.f / std::exp2f(scale);
		return {
		    {
		        .b0 = util::Q31fromFloat(b0 * scale_factor),
		        .b1 = util::Q31fromFloat(b1 * scale_factor),
		        .b2 = util::Q31fromFloat(b2 * scale_factor),
		        .a0 = util::Q31fromFloat(a0 * scale_factor),
		        .a1 = util::Q31fromFloat(a1 * scale_factor),
		        .a2 = util::Q31fromFloat(a2 * scale_factor),
		    },
		    scale,
		};
	}

private:
	auto tied() const { return std::tie(b0, b1, b2, a0, a1, a2); }

public:
	bool operator==(Coefficients<T> const& rhs) const { return this->tied() == rhs.tied(); }
};

Coefficients<float> lowpass(float sample_rate, float frequency, float Q) {
	const auto n = 1.0f / std::tan(std::numbers::pi_v<float> * frequency / sample_rate);
	const auto n2 = n * n;
	const auto c1 = 1.0f / (1.0f + 1.0f / Q * n + n2);

	return {
	    .b0 = c1,
	    .b1 = c1 * 2.f,
	    .b2 = c1,
	    .a0 = 1.0f,
	    .a1 = c1 * 2.f * (1.f - n2),
	    .a2 = c1 * (1.0f - 1.0f / Q * n + n2),
	};
}

Coefficients<float> lowpass(float sample_rate, float frequency) {
	return lowpass(sample_rate, frequency, 1.f / std::numbers::sqrt2_v<float>);
}

Coefficients<float> highpass(float sampleRate, float frequency, float Q) {
	const auto n = std::tan(std::numbers::pi_v<float> * frequency / sampleRate);
	const auto n2 = n * n;
	const auto c1 = 1.0f / (1.0f + 1.0f / Q * n + n2);

	return {
	    .b0 = c1,
	    .b1 = c1 * -2.0f,
	    .b2 = c1,
	    .a0 = 1.0f,
	    .a1 = c1 * 2.0f * (n2 - 1.0f),
	    .a2 = c1 * (1.0f - 1.0f / Q * n + n2),
	};
}

Coefficients<float> highpass(float sampleRate, float frequency) {
	return highpass(sampleRate, frequency, 1.0f / std::sqrt(2.0));
}

Coefficients<float> bandpass(float sampleRate, float frequency, float Q) {
	const auto n = 1.0f / std::tan(std::numbers::pi_v<float> * frequency / sampleRate);
	const auto n2 = n * n;
	const auto c1 = 1.0f / (1.0f + 1.0f / Q * n + n2);

	return {
	    .b0 = c1 * n / Q,
	    .b1 = 0.0f,
	    .b2 = -c1 * n / Q,
	    .a0 = 1.0f,
	    .a1 = c1 * 2.0f * (1.0f - n2),
	    .a2 = c1 * (1.0f - 1.0f / Q * n + n2),
	};
}

Coefficients<float> bandpass(float sampleRate, float frequency) {
	return bandpass(sampleRate, frequency, 1.0f / std::numbers::sqrt2_v<float>);
}

Coefficients<float> notch(float sampleRate, float frequency, float Q) noexcept {
	const auto n = 1.0f / std::tan(std::numbers::pi_v<float> * frequency / sampleRate);
	const auto nSquared = n * n;
	const auto c1 = 1.0f / (1.0f + n / Q + nSquared);

	return {
	    .b0 = c1 * (1.0f + nSquared),
	    .b1 = 2.0f * c1 * (1.0f - nSquared),
	    .b2 = c1 * (1.0f + nSquared),
	    .a0 = 1.0f,
	    .a1 = c1 * 2.0f * (1.0f - nSquared),
	    .a2 = c1 * (1.0f - n / Q + nSquared),
	};
}
Coefficients<float> notch(float sampleRate, float frequency) noexcept {
	return notch(sampleRate, frequency, 1.0f / std::numbers::sqrt2);
}

Coefficients<float> allpass(float sampleRate, float frequency, float Q) noexcept {
	const auto n = 1.0f / std::tan(std::numbers::pi_v<float> * frequency / sampleRate);
	const auto nSquared = n * n;
	const auto c1 = 1.0f / (1.0f + 1.0f / Q * n + nSquared);

	return {
	    .b0 = c1 * (1.0f - n / Q + nSquared),
	    .b1 = c1 * 2.0f * (1.0f - nSquared),
	    .b2 = 1.0f,
	    .a0 = 1.0f,
	    .a1 = c1 * 2.0f * (1.0f - nSquared),
	    .a2 = c1 * (1.0f - n / Q + nSquared),
	};
}

Coefficients<float> allpass(float sampleRate, float frequency) noexcept {
	return allpass(sampleRate, frequency, 1.0f / std::numbers::sqrt2_v<float>);
}
Coefficients<float> lowshelf(float sampleRate, float cutOffFrequency, float Q, float gainFactor) noexcept {
	constexpr auto minDB = -300.f;
	const auto A = std::sqrt(deluge::util::DecibelsToGain(gainFactor, minDB));
	const auto aminus1 = A - 1.0f;
	const auto aplus1 = A + 1.0f;
	const auto omega = (2.0f * std::numbers::pi_v<float> * std::max(cutOffFrequency, 2.0f)) / sampleRate;
	const auto coso = std::cos(omega);
	const auto beta = std::sin(omega) * std::sqrt(A) / Q;
	const auto aminus1TimesCoso = aminus1 * coso;

	return {
	    .b0 = A * (aplus1 - aminus1TimesCoso + beta),
	    .b1 = A * 2.0f * (aminus1 - aplus1 * coso),
	    .b2 = A * (aplus1 - aminus1TimesCoso - beta),
	    .a0 = aplus1 + aminus1TimesCoso + beta,
	    .a1 = -2.0f * (aminus1 + aplus1 * coso),
	    .a2 = aplus1 + aminus1TimesCoso - beta,
	};
}

Coefficients<float> highshelf(float sampleRate, float cutOffFrequency, float Q, float gainFactor) noexcept {
	constexpr auto minDB = -300.f;
	const auto A = std::sqrt(deluge::util::DecibelsToGain(gainFactor, minDB));
	const auto aminus1 = A - 1.0f;
	const auto aplus1 = A + 1.0f;
	const auto omega = (2.0f * std::numbers::pi_v<float> * std::max(cutOffFrequency, 2.0f)) / sampleRate;
	const auto coso = std::cos(omega);
	const auto beta = std::sin(omega) * std::sqrt(A) / Q;
	const auto aminus1TimesCoso = aminus1 * coso;

	return {
	    .b0 = A * (aplus1 + aminus1TimesCoso + beta),
	    .b1 = A * -2.0f * (aminus1 + aplus1 * coso),
	    .b2 = A * (aplus1 + aminus1TimesCoso - beta),
	    .a0 = aplus1 - aminus1TimesCoso + beta,
	    .a1 = 2.0f * (aminus1 - aplus1 * coso),
	    .a2 = aplus1 - aminus1TimesCoso - beta,
	};
}

// Based on Fig. 27 of https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html and JUCE library
Coefficients<float> peakEQ(float sample_rate, float frequency, float Q, float gainFactor) {
	constexpr auto minDB = -300.f;
	const auto A = std::sqrt(std::max(gainFactor, deluge::util::DecibelsToGain(minDB, minDB - 1.f)));
	const auto omega = (2.f * std::numbers::pi_v<float> * std::max(frequency, 2.0f)) / sample_rate; // Fig. 6
	const auto alpha = 0.5f * std::sin(omega) / Q;                                                  // Fig. 7
	const auto c2 = -2.0f * std::cos(omega);
	const auto alphaTimesA = alpha * A;
	const auto alphaOverA = alpha / A;

	return {
	    .b0 = 1.0f + alphaTimesA,
	    .b1 = c2,
	    .b2 = 1.0f - alphaTimesA,
	    .a0 = 1.0f + alphaOverA,
	    .a1 = c2,
	    .a2 = 1.0f - alphaOverA,
	};
}

} // namespace deluge::dsp::blocks::filters::iir
