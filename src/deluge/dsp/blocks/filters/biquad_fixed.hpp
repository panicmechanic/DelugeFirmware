#pragma once

#include "dsp/util/iir_coefficients.hpp"
namespace deluge::dsp::blocks::filters {
class BiquadDF1 {

	using Coefficients = deluge::dsp::util::iir::Coefficients<q31_t>;
};
} // namespace deluge::dsp::blocks::filters
