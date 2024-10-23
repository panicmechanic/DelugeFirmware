#pragma once

#include "argon.hpp"
#include "dsp/stereo_sample.h"
#include <span>

namespace deluge::dsp::blocks {
template <typename T>
class MidSide {
	using sample_type = T::base_type;

public:
	void processStereoBlock(const std::span<T> input, std::span<T> output) {
		using namespace argon;
		constexpr size_t num_parallel = Neon128<sample_type>::lanes;
		const size_t size = input.size();

		size_t vec_size = size - (size % num_parallel);
		for (size_t i = 0; i < vec_size; i += num_parallel) {
			auto [left, right] = Neon128<sample_type>::Load2(&input[i].l); // Load stereo samples
			Neon128<sample_type> mid = left + right;
			Neon128<sample_type> side = left - right;
			Neon128<sample_type>::Store2({mid, side}, &output[i].l);
		}

		// Do remainder that don't fit the vector width
		for (size_t i = vec_size; i < size; ++i) {
			sample_type mid = input[i].l + input[i].r;
			sample_type side = input[i].l - input[i].r;
			output[i] = {mid, side};
		}
	}
};
} // namespace deluge::dsp::blocks
