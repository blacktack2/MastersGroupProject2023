/**
 * @file   Maths.cpp
 * @brief  See Maths.h.
 * 
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "Maths.h"

#include "Vector2.h"
#include "Vector3.h"

using namespace NCL::Maths;

void NCL::Maths::ScreenBoxOfTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector2& topLeft, Vector2& bottomRight) {
	topLeft.x = std::min(v0.x, std::min(v1.x, v2.x));
	topLeft.y = std::min(v0.y, std::min(v1.y, v2.y));

	bottomRight.x = std::max(v0.x, std::max(v1.x, v2.x));
	bottomRight.y = std::max(v0.y, std::max(v1.y, v2.y));
}

int NCL::Maths::ScreenAreaOfTri(const Vector3& a, const Vector3& b, const Vector3& c) {
	int area = (int)(((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
					 ((b.x * a.y) + (c.x * b.y) + (a.x * c.y)));
	return area >> 1; // (area / 2)
}

float NCL::Maths::FloatAreaOfTri(const Vector3& a, const Vector3& b, const Vector3& c) {
	float area = ((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				 ((b.x * a.y) + (c.x * b.y) + (a.x * c.y));
	return area * 0.5f;
}

float NCL::Maths::CrossAreaOfTri(const Vector3& a, const Vector3& b, const Vector3& c) {
	Vector3 area = Vector3::Cross(a - b, a - c);
	return area.Length() * 0.5f;
}
