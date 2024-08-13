#pragma once

#include <cmath>
#include <memory.h>
#include <limits>

namespace Math
{
	const float PI = 3.141592653f;
	const float TwoPI = PI * 2.0f;
	const float PIOver2 = PI / 2.0f;
	const float Infinity = std::numeric_limits<float>::infinity();
	const float NegativeInfinity = -std::numeric_limits<float>::infinity();

	inline float ToRadians(float degrees) {
		return degrees * PI / 180.0f;
	}

	inline float ToDegrees(float radians) {
		return radians * 180.0f / PI;
	}

	inline bool NearZero(float val, float epsilon = 0.001f) {
		if (fabs(val) <= epsilon) { return true; }
		else { return false; }
	}

	template <typename T>
	T Max(const T& a, const T& b) {
		return (a < b ? b : a);
	}
	
	template <typename T>
	T Min(const T& a, const T& b) {
		return (a < b ? a : b);
	}

	template <typename T>
	T Clamp(const T& value, const T& lower, const T& upper) {
		return Min(upper, Max(lower, value));
	}

	inline float Abs(float value) { return fabs(value); }
	inline float Cos(float angle) { return cosf(angle); }
	inline float Sin(float angle) { return sinf(angle); }
	inline float Tan(float angle) { return tanf(angle); }
	inline float Acos(float value) { return acosf(value); }
	inline float Atan2(float y, float x) { return atan2f(y, x); }
	inline float Cot(float angle) { return 1.0f / Tan(angle); }
	inline float Lerp(float a, float b, float f) { return a + f * (b - a); }
	inline float Sqrt(float value) { return sqrtf(value); }
	inline float Fmod(float number, float denom) { return fmod(number, denom); }
}