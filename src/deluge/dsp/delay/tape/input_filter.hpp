#pragma once

#include "dsp/blocks/filters/iir/linkwitz_riley.hpp"
#include "dsp/util/stereo_delay_line.hpp"
namespace deluge::dsp::delay::tape {
class InputFilter {
	static constexpr float min_freq = 20.0f;
	static constexpr float max_freq = 22000.0f;

public:
	struct Config{
		bool bypass = false;
		float lowcut = 200.f;
		float highcut = 10000.f;
		bool makeup = false;
	};

private:
	blocks::filters::iir::LinkwitzRileyStereo lowcut_filter_; // 20Hz - 2kHz; center: 200Hz
	blocks::filters::iir::LinkwitzRileyStereo highcut_filter_; // 2kHz - 22kHz; center: 10k

	util::StereoFloatDelayLine<(1 << 21)> makeup_delay_;
};
} // namespace deluge::dsp::delay::tape
