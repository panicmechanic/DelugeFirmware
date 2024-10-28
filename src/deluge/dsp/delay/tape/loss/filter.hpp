
#pragma once
#include "NE10_types.h"
#include "azimuth.hpp"
#include "definitions_cxx.hpp"
#include "dsp/blocks/filters/iir/biquad_float.hpp"
#include "dsp/blocks/filters/iir/coefficients.hpp"
#include "util/unit_convernsions.h"

namespace deluge::dsp::delay::tape::loss {
class LossFilter {
	struct Params {
		float speed;
		float spacing;
		float thickness;
		float gap;
		float azimuth;
	};

private:
	using Biquad = blocks::filters::iir::BiquadDF2TStereo<2>;
	using Coefficients = blocks::filters::iir::Coefficients<float>;

	static Coefficients calcHeadBumpFilter(float speedIps, float gapMeters, float fs) {
		auto bumpFreq = deluge::util::InchesToMeters(speedIps) / (gapMeters * 500.0f);
		auto gain = std::max(1.5f * (1000.0f - std::abs(bumpFreq - 100.0f)) / 1000.0f, 1.0f);
		return blocks::filters::iir::peakEQ(fs, bumpFreq, 2.0f, gain);
	}

	Coefficients calcCoefs() {
		// Set freq domain multipliers
		auto& H = Hcoefs;
		for (int k = 0; k < order / 2; k++) {
			const auto freq = (float)k * binWidth;
			const auto waveNumber =
			    2 * std::numbers::pi_v<float> * std::max(freq, 20.0f) / deluge::util::InchesToMeters(params_->speed);
			const auto thickTimesK = waveNumber * (params_->thickness * (float)1.0e-6);
			const auto kGapOverTwo = waveNumber * (params_->gap * (float)1.0e-6) / 2.0f;

			H[k] = std::exp(-waveNumber * (params_->spacing * (float)1.0e-6)); // Spacing loss
			H[k] *= (1.0f - std::exp(-thickTimesK)) / thickTimesK;             // Thickness loss
			H[k] *= std::sin(kGapOverTwo) / kGapOverTwo;                       // Gap loss
			H[order - k - 1] = H[k];
		}

		// Create time domain filter signal
		auto& h = currentCoefs;
		for (size_t n = 0; n < order / 2; n++) { // 0 to 32
			const auto idx = (size_t)order / 2 + (size_t)n;
			for (size_t k = 0; k < order; k++) // 0 to 64
				h[idx] += Hcoefs[k] * std::cos(2 * std::numbers::pi_v<float> * (float)k * (float)n / (float)order);

			h[idx] /= (float)order;
			h[order / 2 - n] = h[idx];
		}

		// compute head bump filters
		return calcHeadBumpFilter(params_->speed, params_->gap * (float)1.0e-6, fs);
	}

	std::array<ne10_fir_instance_f32_t, 2> filters;
	Biquad bump_filter_;

	int activeFilter = 0;
	int fadeCount = 0;
	int fadeLength = 1024;
	std::span<float> fadeBuffer;

	Params* params_;

	float prevSpeed = 0.5f;
	float prevSpacing = 0.5f;
	float prevThickness = 0.5f;
	float prevGap = 0.5f;

	static constexpr size_t order = 64;

	static constexpr float fs = kSampleRate;
	static constexpr float fsFactor = 1.0f;
	static constexpr float binWidth = fs / order;

	std::array<float, order> currentCoefs;
	std::array<float, order> Hcoefs;

	Azimuth azimuthProc;
};
} // namespace deluge::dsp::delay::tape::loss
