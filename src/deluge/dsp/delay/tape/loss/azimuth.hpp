#pragma once

#include "dsp/stereo_sample.h"
#include "dsp/util/interpolated_value.hpp"
#include "dsp/util/stereo_delay_line.hpp"
#include "util/unit_convernsions.h"
#include <cstddef>
#include <span>

namespace deluge::dsp::delay::tape::loss {
constexpr size_t max_size = size_t(1 << 18) - 1;
class Azimuth {
	static constexpr size_t num_channels = 2;

public:
	Azimuth() = default;

	void init(float sample_rate) {
		sample_rate_ = sample_rate;
		delay_.Init();
		delay_sample_smoothed_[0].Reset(sample_rate, 0.05);
		delay_sample_smoothed_[1].Reset(sample_rate, 0.05);
	}

	void processBlock(float angle_deg, float tape_speed_ips, float tape_width, std::span<StereoFloatSample> in_out) {
		if (angle_deg_ != angle_deg || tape_speed_ips_ != tape_speed_ips || tape_width_ != tape_width) {
			angle_deg_ = angle_deg;
			tape_speed_ips_ = tape_speed_ips;
			tape_width_ = tape_width;
			recalculateDelay();
		}
		for (StereoFloatSample& sample : in_out) {
			auto delay0 = (delay_sample_smoothed_[0].is_interpolating()) ? delay_sample_smoothed_[0].Next() : 0.f;
			auto delay1 = (delay_sample_smoothed_[1].is_interpolating()) ? delay_sample_smoothed_[1].Next() : 0.f;
			delay_.Write(sample);
			sample = delay_.ReadLagrange3rd(delay0, delay1);
		}
	}

private:
	void recalculateDelay() {
		const auto delay_idx = static_cast<size_t>(angle_deg_ < 0.0f);
		const float tape_speed = deluge::util::InchesToMeters(tape_speed_ips_);
		const float azimuth_angle = deluge::util::DegreeToRad(std::abs(angle_deg_));

		const float delay_distance = deluge::util::InchesToMeters(tape_width_) * std::sin(azimuth_angle);
		const float delay_sample = (delay_distance * tape_speed) * sample_rate_;

		delay_sample_smoothed_[delay_idx].set_target(delay_sample);
		delay_sample_smoothed_[1 - delay_idx].set_target(0.0f);
	}

	util::StereoFloatDelayLine<max_size> delay_;
	util::InterpolatedValue<float> delay_sample_smoothed_[2];
	float angle_deg_;
	float tape_speed_ips_;
	float sample_rate_;
	float tape_width_; // inches
};
} // namespace deluge::dsp::delay::loss
