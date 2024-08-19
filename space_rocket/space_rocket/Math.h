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

class Vector2
{
public:
	float x;
	float y;

	Vector2() : x(0.0f), y(0.0f) {}
	explicit Vector2(float inX, float inY) : x(inX), y(inY) {}

	void Set(float inX, float inY) {
		x = inX;
		y = inY;
	}

	friend Vector2 operator+(const Vector2& a, const Vector2& b) {
		return Vector2(a.x + b.x, a.y + b.y);
	}

	friend Vector2 operator-(const Vector2& a, const Vector2& b) {
		return Vector2(a.x - b.x, a.y - b.y);
	}

	friend Vector2 operator*(const Vector2& vec, float scaler) {
		return Vector2(vec.x * scaler, vec.y * scaler);
	}

	friend Vector2 operator*(float scaler, const Vector2& vec) {
		return Vector2(vec.x * scaler, vec.y * scaler);
	}

	Vector2& operator*=(float scaler) {
		x *= scaler;
		y *= scaler;
		return *this;
	}

	Vector2& operator+=(const Vector2& right) {
		x += right.x;
		y += right.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& right) {
		x -= right.x;
		y -= right.y;
		return *this;
	}

	float LengthSq() const {
		return (x * x + y * y);
	}

	float Length() const {
		return (Math::Sqrt(LengthSq()));
	}

	void Normalize() {
		float length = Length();
		x /= length;
		y /= length;
	}

	static Vector2 Normalize(const Vector2& vec) {
		Vector2 temp = vec;
		temp.Normalize();
		return temp;
	}

	static float Dot(const Vector2& lhs, const Vector2& rhs) { return ((lhs.x * rhs.x) + (lhs.y * rhs.y)); }
	static Vector2 Lerp(const Vector2& lhs, const Vector2& rhs, float f) { return Vector2(lhs + f * (rhs - lhs)); }
	static Vector2 Reflect(const Vector2& v, const Vector2& n) { return v - 2.0f * Vector2::Dot(v, n) * n; }

	/* Transform vector by matrix */
	static const Vector2 Zero;
	static const Vector2 UnitX;
	static const Vector2 UnitY;
	static const Vector2 NegativeUnitX;
	static const Vector2 NegativeUnitY;
};