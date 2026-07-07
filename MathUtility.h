#pragma once
#include "Vector.h"
Vector2 Lerp(const Vector2& a, const Vector2& b, float t) {
	Vector2 result;
	result.x = a.x + (b.x - a.x) * t;
	result.y = a.y + (b.y - a.y) * t;
	return result;
}