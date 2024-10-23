#pragma once

#include "argon.hpp"
#include "dsp/stereo_sample.h"
#include <span>

namespace deluge::dsp::blocks {

template <typename T>
class Sum {
	using repr = typename T::base_type;

public:
	[[gnu::always_inline]] inline static void processStereoBlock(const std::span<T> input1, const std::span<T> input2,
	                                                             std::span<T> output) {
		using namespace argon;
		constexpr size_t num_parallel = Neon128<repr>::lanes;
		const size_t size = std::min(input1.size(), input2.size());

		size_t vec_size = size - (size % num_parallel);
		for (size_t i = 0; i < vec_size; i += num_parallel) {
			auto [left1, right1] = Neon128<repr>::Load2(&input1[i].l); // Load stereo samples
			auto [left2, right2] = Neon128<repr>::Load2(&input2[i].l); // Load stereo samples
			Neon128<repr>::Store2({left1 + left2, right1 + right2}, &output[i].l);
		}

		// Do remainder that don't fit the vector width
		for (size_t i = vec_size; i < size; ++i) {
			output[i].l = input1[i].l + input2[i].l;
			output[i].r = input1[i].r + input2[i].r;
		}
	}
};

} // namespace deluge::dsp::blocks
