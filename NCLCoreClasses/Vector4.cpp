/**
 * @file   Vector4.cpp
 * @brief  See Vector4.h.
 * 
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "Vector4.h"

#include "Vector3.h"
#include "Vector2.h"

#include <algorithm>

using namespace NCL::Maths;

Vector4::Vector4(const Vector2& v2, float newZ, float newW) : x(v2.x), y(v2.y), z(newZ), w(newW) {
}

Vector4::Vector4(float newX, const Vector2& v2, float newW) : x(newX), y(v2.x), z(v2.y), w(newW) {
}

Vector4::Vector4(float newX, float newY, const Vector2& v2) : x(newX), y(newY), z(v2.x), w(v2.y) {
}

Vector4::Vector4(const Vector2& v2a, const Vector2& v2b) : x(v2a.x), y(v2a.y), z(v2b.x), w(v2b.y) {
}

Vector4::Vector4(const Vector3& v3, float newW) : x(v3.x), y(v3.y), z(v3.z), w(newW) {
}

Vector4::Vector4(float newX, const Vector3& v3) : x(newX), y(v3.x), z(v3.y), w(v3.z) {
}
