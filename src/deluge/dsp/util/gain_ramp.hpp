#pragma once

#include "dsp/stereo_sample.h"
#include "util/fixedpoint.h"
#include <ranges>
#include <argon.hpp>

namespace deluge::dsp::util {
class GainRamp {
public:
	GainRamp(float start, float end) : start_(start), end_(end) {};

	void processStereoBlock(std::span<StereoSampleFloat> in_out) {}

    void processStereoBlock(std::span<StereoSampleFloat> in, std::span<StereoSampleFloat> out) {
		float step = (end_ - start_) / (in.size() - 1);
		float current = start_;
		for (size_t i = 0; i < in.size(); i++) {
			argon::Neon64<float> in_sample_neon{&in[i].l}; // will load both channels
			argon::Neon64<float> out_sample_neon = in_sample_neon * argon::Neon64<float>{current};
			out_sample_neon.Store(&out[i].l); // store in output
			current += step;
		}
	}

	void processBlock(std::span<float> in_out) { processBlock(in_out, in_out); }

	void processBlock(const std::span<float> in, std::span<float> out) {
		float step = (end_ - start_) / (in.size() - 1);
		float current = start_;
		for (size_t i = 0; i < in.size(); i++) {
			out[i] = in[i] * current;
			current += step;
		}
	}

private:
	float start_;
	float end_;
};
} // namespace deluge::dsp::util
