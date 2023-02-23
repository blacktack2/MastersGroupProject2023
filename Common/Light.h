/**
 * @file   Light.h
 * @brief
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "Vector3.h"
#include "Vector4.h"

using namespace NCL::Maths;

namespace NCL::CSC8503 {
	struct Light {
		Light(const Vector4& position = Vector4(0.0f), const Vector4& colour = Vector4(1.0f),
			float radius = 0.0f, const Vector3& direction = Vector3(1.0f, 0.0f, 0.0f), float angle = 360.0f) :
			position(position), colour(colour), radius(radius), direction(direction), angle(angle) {}

		Vector4 position;
		Vector4 colour;
		float radius;
		Vector3 direction;
		float angle;
	};
}
