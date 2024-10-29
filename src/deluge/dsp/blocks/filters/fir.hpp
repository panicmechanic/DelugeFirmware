#pragma once

#include "NE10.h"
#include "NE10_types.h"
#include "definitions_cxx.hpp"
#include "dsp/stereo_sample.h"
#include <array>
#include <cstddef>
#include <span>

namespace deluge::dsp::blocks::filters {
template <size_t num_taps>
class FIRFilter {
public:
	FIRFilter()
	    : instance_{
	        .numTaps = num_taps,
	        .pState = &state_,
	        .pCoeffs = &coeffs_,
	    } {}

	void process(const std::span<float> input, std::span<float> output) {
		ne10_fir_float(&instance_, input.data(), output.data(), input.size());
	}

private:
	ne10_fir_instance_f32_t instance_;
	std::array<float, num_taps> coeffs_;
	std::array<float, num_taps + kMaxBlockSize - 1> state_;
};

template <size_t num_taps>
class FIRFilterStereo {
public:
	FIRFilterStereo()
	    : instance_l_{
	        .numTaps = num_taps,
	        .pState = &state_l_,
	        .pCoeffs = &coeffs_,
	    }, instance_r_{
	        .numTaps = num_taps,
	        .pState = &state_r_,
	        .pCoeffs = &coeffs_,
	    } {}

	void process(const std::span<StereoFloatSample> input, std::span<StereoFloatSample> output) {
		float input_l[input.size()]; // vla
		float input_r[input.size()]; // vla

		float output_l[output.size()]; // vla
		float output_r[output.size()]; // vla

		stereo_deinterleave(input, input_l, input_r);

		ne10_fir_float(&instance_l_, input_l, output_l, input.size());
		ne10_fir_float(&instance_r_, input_r, output_r, input.size());

		stereo_interleave(output_l, output_r, output);
	}

private:
	ne10_fir_instance_f32_t instance_l_;
	ne10_fir_instance_f32_t instance_r_;
	std::array<float, num_taps> coeffs_;
	std::array<float, num_taps + kMaxBlockSize - 1> state_l_;
	std::array<float, num_taps + kMaxBlockSize - 1> state_r_;
};
} // namespace deluge::dsp::blocks::filters
