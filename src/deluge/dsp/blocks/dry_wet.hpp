#pragma once

#include "argon.hpp"
#include "dsp/stereo_sample.h"
#include <span>
namespace deluge::dsp::blocks {
struct DryWet {
	float ratio; // Dry : Wet (0, 1]

	void processBlock(const std::span<float> dry, const std::span<float> wet, std::span<float> out) const {
		constexpr size_t lanes = argon::Neon128<float>::lanes;

		assert(dry.size() == wet.size());

		size_t size = dry.size();
		size_t vec_size = size - (size % lanes);

		auto wet_multiplicand = ratio;
		auto dry_multiplicand = 1.f - ratio;

		for (size_t i = 0; i < vec_size; i += lanes) {
			argon::Neon128<float> dry_vec{&dry[i]};                                        // Load mono dry samples
			argon::Neon128<float> wet_vec{&wet[i]};                                        // Load mono wet samples
			auto out_sample = (dry_vec * dry_multiplicand) + (wet_vec * wet_multiplicand); // Apply gain
			out_sample.Store(&out[i]);                                                     // Store mono samples
		}

		// Do remainder that don't fit the vector width
		for (size_t i = vec_size; i < size; ++i) {
			out[i] = (dry[i] * dry_multiplicand) + (wet[i] * wet_multiplicand);
		}
	}

	void processStereoBlock(const std::span<StereoFloatSample> dry, const std::span<StereoFloatSample> wet,
	                        std::span<StereoFloatSample> out) const {
		constexpr size_t lanes = argon::Neon128<float>::lanes / 2; // because stereo

		assert(dry.size() == wet.size());

		size_t size = dry.size();
		size_t vec_size = size - (size % lanes);

		auto wet_multiplicand = ratio;
		auto dry_multiplicand = 1.f - ratio;

		for (size_t i = 0; i < vec_size; i += lanes) {
			argon::Neon128<float> dry_vec{&dry[i].l};                                      // Load stereo dry samples
			argon::Neon128<float> wet_vec{&wet[i].l};                                      // Load stereo wet samples
			auto out_sample = (dry_vec * dry_multiplicand) + (wet_vec * wet_multiplicand); // Apply gain
			out_sample.Store(&out[i].l);                                                   // Store stereo samples
		}

		// Do remainder that don't fit the vector width
		for (size_t i = vec_size; i < size; ++i) {
			argon::Neon64<float> dry_vec{&dry[i].l};                                       // Load stereo dry sample
			argon::Neon64<float> wet_vec{&wet[i].l};                                       // Load stereo wet sample
			auto out_sample = (dry_vec * dry_multiplicand) + (wet_vec * wet_multiplicand); // Apply gain
			out_sample.Store(&out[i].l);                                                   // Store stereo sample
		}
	}
};
} // namespace deluge::dsp::blocks
