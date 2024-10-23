#pragma once

#include "dsp/stereo_sample.h"
#include "dsp/util/interpolated_value.hpp"
#include <argon.hpp>
#include <span>

namespace deluge::dsp::blocks {
class Pan {
public:
	void processBlock(std::span<float> input, std::span<StereoFloatSample> output) {
		using namespace argon;
		constexpr size_t num_parallel = Neon128<float>::lanes;
		const size_t size = input.size();

		pan_.set_steps(size);

		size_t vec_size = size - (size % num_parallel);
		for (size_t i = 0; i < vec_size; i += num_parallel) {
			auto pans = pan_.NextN<num_parallel>();
 			auto pan_vec = Neon128<float>::Load(pans.data());
			auto input_samples = Neon128<float>::Load(&input[i]); // Load stereo samples
			Neon128<float> left = input_samples * pan_vec;
			Neon128<float> right = input_samples * (1.f - pan_vec);
			Neon128<float>::Store2({left, right}, &output[i].l);
		}

		// Do remainder that don't fit the vector width
		for (size_t i = vec_size; i < size; ++i) {
			float pan = pan_.Next();
			output[i].l = input[i] * pan;
			output[i].r = input[i] * (1.f - pan);
		}
	}

	// range: 0 - 1.f
	void setPan(float pan) { pan_.set_target(pan); }
	float getPan() { return pan_.target(); }

private:
	util::InterpolatedValue<float> pan_;
};
} // namespace deluge::dsp::blocks
