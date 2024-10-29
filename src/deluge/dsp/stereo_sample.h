/*
 * Copyright © 2014 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

// Ok, creating a class for this is absolutely stupid, but I was a noob at the time! It doesn't add any performance
// overhead though.

#pragma once

#include "util/fixedpoint.h"
#include "util/functions.h"
#include <span>
#include <argon.hpp>

struct StereoSample {
	inline void addMono(q31_t sampleValue) {
		l += sampleValue;
		r += sampleValue;
	}

	// Amplitude is probably Q2.29?
	inline void addPannedMono(q31_t sampleValue, int32_t amplitudeL, int32_t amplitudeR) {
		l += (multiply_32x32_rshift32(sampleValue, amplitudeL) << 2);
		r += (multiply_32x32_rshift32(sampleValue, amplitudeR) << 2);
	}

	inline void addStereo(q31_t sampleValueL, q31_t sampleValueR) {
		l += sampleValueL;
		r += sampleValueR;
	}

	[[gnu::always_inline]] constexpr StereoSample operator+(const StereoSample& rhs) const {
		return StereoSample{
		    .l = l + rhs.l,
		    .r = r + rhs.r,
		};
	}
	[[gnu::always_inline]] constexpr StereoSample& operator+=(const StereoSample& rhs) {
		l = l + rhs.l;
		r = r + rhs.r;
		return *this;
	}

	// Amplitude is probably Q2.29?
	inline void addPannedStereo(q31_t sampleValueL, q31_t sampleValueR, int32_t amplitudeL, int32_t amplitudeR) {
		l += (multiply_32x32_rshift32(sampleValueL, amplitudeL) << 2);
		r += (multiply_32x32_rshift32(sampleValueR, amplitudeR) << 2);
	}

	q31_t l = 0;
	q31_t r = 0;

	using base_type = q31_t;
};

struct StereoFloatSample {
	float l = 0.f;
	float r = 0.f;

	using base_type = float;
};

void stereo_deinterleave(const std::span<StereoFloatSample> stereo, std::span<float> left, std::span<float> right) {
	using namespace argon;
	size_t block_end = stereo.size() & 0x03;
	size_t i;
	for (i = 0; i < block_end; i += 4) {
		auto [l, r] = Neon128<float>::Load2(&stereo[i].l);
		l.Store(&left[i]);
		r.Store(&right[i]);
	}
	for (; i < stereo.size(); ++i) {
		left[i] = stereo[i].l;
		right[i] = stereo[i].r;
	}
}


void stereo_interleave(const std::span<float> left, const std::span<float> right, std::span<StereoFloatSample> stereo) {
	using namespace argon;
	size_t block_end = stereo.size() & 0x03;
	size_t i;
	for (i = 0; i < block_end; i += 4) {
		auto l = Neon128<float>::Load(&left[i]);
		auto r = Neon128<float>::Load(&right[i]);
		Neon128<float>::Store2({l, r}, &stereo[i].l)
	}
	for (; i < stereo.size(); ++i) {
		stereo[i].l = left[i];
		stereo[i].r = right[i];
	}
}
