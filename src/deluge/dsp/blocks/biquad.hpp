#pragma once

#include "deluge/dsp/stereo_sample.h"
#include <argon.hpp>
#include <array>
#include <cstdint>
#include <span>
#include "dsp/util/iir_coefficients.hpp"

namespace deluge::dsp::blocks {
class StereoBiquad {
public:
	using Coefficients = deluge::dsp::util::iir::Coefficients;
	using State = std::array<float, 4>;

	struct Config {
		uint8_t num_stages;
		std::span<State> state;
		std::span<Coefficients> coefficients;
	};

	StereoBiquad(Config c) : config_{c} {}

	// StereoBiquad
	void processStereoBlock(const std::span<StereoFloatSample> in, std::span<StereoFloatSample> out) {
		using namespace argon;
		std::array<float, 6> scratch; // layout : [d1a d1b d2a d2b 0 0]
		Neon128<float> stateVec0, stateVec1, inVec;

		auto it = in.begin();
		for (size_t stage = 0; stage < config_.num_stages; ++stage) {
			State& state = config_.state[stage];
			Coefficients& coeffs = config_.coefficients[stage];

			Neon128<float> aCoeffs{{coeffs.a1, coeffs.a1, coeffs.a2, coeffs.a2}};
			Neon128<float> bCoeffs{{coeffs.b1, coeffs.b1, coeffs.b2, coeffs.b2}};

			// read state into scratch
			*(float32x4_t*)scratch.data() = *(float32x4_t*)state.data();

			for (size_t i = 0; i < out.size(); ++i) {
				/*
				 * step 1
				 *
				 * 0   | acc1a = xn1a * b0 + d1a
				 * 1   | acc1b = xn1b * b0 + d1b
				 * 2   | acc1a = xn1a * b0 + d1a
				 * 3   | acc1b = xn1b * b0 + d1b
				 */

				// load {d1a, d1b, d1a, d1b}
				stateVec0 = {scratch[0], scratch[1], scratch[0], scratch[1]};

				// load {in0 in1 in0 in1}
				inVec = {it[i].l, it[i].r, it[i].l, it[i].r};

				stateVec0 = stateVec0.MultiplyAdd(inVec, coeffs.b0);
				out[i] = {stateVec0[0], stateVec0[1]};

				/*
				 * step 2
				 *
				 * 0  | d1a = b1 * xn1a  +  a1 * acc1a  +  d2a
				 * 1  | d1b = b1 * xn1b  +  a1 * acc1b  +  d2b
				 * 2  | d2a = b2 * xn1a  +  a2 * acc1a  +  0
				 * 3  | d2b = b2 * xn1b  +  a2 * acc1b  +  0
				 */
				stateVec1.Load(&scratch[2]); // load {d2a, d2b, 0, 0}
				stateVec1 = stateVec1.MultiplyAdd(stateVec0, aCoeffs);
				stateVec1 = stateVec1.MultiplyAdd(inVec, bCoeffs);
				stateVec1.Store(scratch.data());
			}

			stateVec1.Store(state.data()); // Store the updated state variables into the persistent state storage

			it = out.begin(); // The current stage output is given as the input to the next stage
		}
	}

private:
	Config config_;
};
}
