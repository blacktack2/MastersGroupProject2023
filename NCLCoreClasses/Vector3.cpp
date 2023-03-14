/**
 * @file   Vector3.cpp
 * @brief  See Vector3.h.
 * 
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "Vector3.h"

#include "Vector4.h"
#include "Vector2.h"

using namespace NCL::Maths;

Vector3::Vector3(const Vector2& v2, float newZ) : x(v2.x), y(v2.y), z(newZ) {
}

Vector3::Vector3(const Vector4& v4) : x(v4.x), y(v4.y), z(v4.z) {
}
