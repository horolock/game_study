#pragma once

#include <cmath>

/*
* 2D Vector class
*/
class Vector2
{
public:
	Vector2() : x(0.0f), y(0.0f) {}
	explicit Vector2(float inX, float inY)
		: x(inX), y(inY) {}

	void Set(float inX, float inY)
	{
		x = inX;
		y = inY;
	}

	friend Vector2 operator+(const Vector2& lhs, const Vector2& rhs) { return Vector2(lhs.x + rhs.x, lhs.y + rhs.y); }
	friend Vector2 operator-(const Vector2& lhs, const Vector2& rhs) { return Vector2(lhs.x - rhs.x, lhs.y - rhs.y); }

	/* Vector Length */
	float LengthSquared() const { return ((x * x) + (y * y)); }
	float Length() const { return sqrtf(LengthSquared()); }
	
public:
	float x;
	float y;
};