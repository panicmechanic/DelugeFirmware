#pragma once

#include "dsp/stereo_sample.h"
#include <argon.hpp>

namespace deluge::dsp::blocks {
struct GainRamp {
	GainRamp(float start, float end) : start_{start}, end_{end} {}

	void processBlock(const std::span<float> in, std::span<float> out) const {
		constexpr size_t lanes = argon::Neon128<float>::lanes;
		float single_step = (end_ - start_) / (in.size() - 1);

		// base algorithm
		if (in.size() < lanes) {
			float current = start_;
			for (size_t i = 0; i < in.size(); ++i) {
				out[i] = in[i] * current; // VCA: [signal] * [amplitude]
				current += single_step;
			}
			return;
		}

		// NEON-accelerated version
		argon::Neon128<float> current = {
		    start_,
		    start_ + single_step,
		    start_ + (single_step * 2),
		    start_ + (single_step * 3),
		};

		size_t size = in.size();

		size_t vec_size = size - (size % lanes);

		argon::Neon128<float> step{single_step * lanes};

		for (size_t i = 0; i < vec_size; i += lanes) {
			argon::Neon128<float> in_sample{&in[i]}; // Load four mono samples
			auto out_sample = in_sample * current;   // Apply gain
			out_sample.Store(&out[i]);               // Store four mono samples
			current += step;                         // Move to next segment of ramp
		}

		// Do remainder that don't fit the vector width
		float single_current = current[lanes - 1] + single_step;
		for (size_t i = vec_size; i < size; ++i) {
			out[i] = in[i];
			single_current += single_step;
		}
	}

	void processStereoBlock(std::span<StereoFloatSample> in, std::span<StereoFloatSample> out) {
		// Must be greater than 2 to be accelerated (and if less than two: why run at all?)
		// assert(in.size() > 2);

		constexpr size_t num_parallel = argon::Neon128<float>::lanes / 2;
		const size_t size = in.size();
		float single_step = (end_ - start_) / (size - 1);

		argon::Neon128<float> current = {
		    start_,
		    start_,
		    start_ + single_step,
		    start_ + single_step,
		};

		size_t vec_size = size - (size % num_parallel);
		argon::Neon128<float> step{single_step * 2};

		for (size_t i = 0; i < vec_size; i += num_parallel) {
			argon::Neon128<float> in_sample{&in[i].l}; // Load two stereo samples
			auto out_sample = in_sample * current;     // Apply gain
			out_sample.Store(&out[i].l);               // Store two stereo samples
			current += step;                           // Move to next segment of ramp
		}

		// Do remainder that don't fit the vector width
		float single_current = current[3] + single_step;
		for (size_t i = vec_size; i < size; ++i) {
			out[i] = in[i];
			single_current += single_step;
		}
	}

private:
	float start_;
	float end_;
};
} // namespace deluge::dsp::blocks
