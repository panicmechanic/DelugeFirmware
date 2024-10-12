#pragma once
#include <cmath>
#include <concepts>
#include <cstddef>
#include <numbers>

namespace deluge::util {

template <std::floating_point T>
constexpr T InchesToMeters(T inches) {
  return inches * 0.0254;
}

template <std::floating_point T>
constexpr T MetersToInches(T meters) {
  return meters / 0.0254;
}

template <std::floating_point T>
constexpr T DegreeToRad(T degrees) {
  return degrees * std::numbers::pi_v<T> / 180.0;
}

inline float DecibelsToGain(float decibels, float lower_limit = -100.f) {
  return (decibels > lower_limit) ? std::pow(10.0, decibels * 0.05f) : 0.0f;
}

template <size_t accuracy = 3>
constexpr float SemitonesToRatio(float semitones) {
  return pow2fast<accuracy>(semitones / 12.f);
}

}
