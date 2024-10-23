#pragma once
#include "argon.hpp"
#include "dsp/stereo_sample.h"
#include <span>

namespace deluge::dsp::blocks {
struct GainConstant {
	GainConstant(float amount) : amount_{amount} {}

	void processBlock(const std::span<float> in, std::span<float> out) const {
		constexpr size_t lanes = argon::Neon128<float>::lanes;

		size_t size = in.size();
		size_t vec_size = size - (size % lanes);
		argon::Neon128<float> amount_vec{amount_};

		for (size_t i = 0; i < vec_size; i += lanes) {
			argon::Neon128<float> in_vec{&in[i]};  // Load mono samples
			auto out_sample = in_vec * amount_vec; // Apply gain
			out_sample.Store(&out[i]);             // Store mono samples
		}

		// Do remainder that don't fit the vector width
		for (size_t i = vec_size; i < size; ++i) {
			out[i] = in[i] * amount_;
		}
	}

	void processStereoBlock(const std::span<StereoFloatSample> in, std::span<StereoFloatSample> out) const {
		constexpr size_t lanes = argon::Neon128<float>::lanes / 2; // because stereo

		size_t size = in.size();
		size_t vec_size = size - (size % lanes);
		argon::Neon128<float> amount_vec{amount_};

		for (size_t i = 0; i < vec_size; i += lanes) {
			argon::Neon128<float> in_vec{&in[i].l}; // Load stereo samples
			auto out_sample = in_vec * amount_vec;  // Apply gain
			out_sample.Store(&out[i].l);            // Store stereo samples
		}

		argon::Neon64<float> amount_vec_small{amount_};

		// Do remainder that don't fit the vector width
		for (size_t i = vec_size; i < size; ++i) {
			argon::Neon64<float> in_vec{&in[i].l};       // Load stereo sample
			auto out_sample = in_vec * amount_vec_small; // Apply gain
			out_sample.Store(&out[i].l);                 // Store stereo sample
		}
	}

private:
	float amount_;
};
} // namespace deluge::dsp::blocks
