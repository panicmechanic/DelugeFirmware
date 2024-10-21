#pragma once
#include "dsp/blocks/gain_constant.hpp"
#include "dsp/blocks/gain_ramp.hpp"
namespace deluge::dsp::processors {
using namespace blocks;
struct Gain {
	Gain() = default;
	Gain(float gain) : gain_{gain}, prev_gain_{gain} {};

	void processBlock(const std::span<float> in, std::span<float> out) {
		if (gain_ == prev_gain_) {
			GainConstant{gain_}.processBlock(in, out);
			return;
		}
		GainRamp{prev_gain_, gain_}.processBlock(in, out);
		prev_gain_ = gain_;
	}

	void processStereoBlock(const std::span<StereoFloatSample> in, std::span<StereoFloatSample> out) {
		if (gain_ == prev_gain_) {
			GainConstant{gain_}.processStereoBlock(in, out);
			return;
		}
		GainRamp{prev_gain_, gain_}.processStereoBlock(in, out);
		prev_gain_ = gain_;
	}

	void setGain(float gain) {
		if (gain == gain_) {
			return;
		}

		prev_gain_ = gain_;
		gain_ = gain;
	}

private:
	float gain_ = 1.f;
	float prev_gain_ = gain_;
};
} // namespace deluge::dsp::processors
