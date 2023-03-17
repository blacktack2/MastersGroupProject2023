/**
 * @file   Vector2.cpp
 * @brief  See Vector2.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "Vector2.h"

#include "Vector4.h"
#include "Vector3.h"

using namespace NCL::Maths;

Vector2::Vector2(const Vector3& v3) : x(v3.x), y(v3.y){
}

Vector2::Vector2(const Vector4& v4) : x(v4.x), y(v4.y) {
}

Vector2 Vector2::Normalised() const {
	Vector2 temp(*this);
	temp.Normalise();
	return temp;
}

void Vector2::Normalise() {
	float length = LengthSquared();

	if (length != 0.0f) {
		length = 1.0f / sqrt(length);
		x = x * length;
		y = y * length;
	}
}
