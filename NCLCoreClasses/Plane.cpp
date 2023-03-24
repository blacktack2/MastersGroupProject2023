/**
 * @file   Plane.cpp
 * @brief  See Plane.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "Plane.h"

using namespace NCL::Maths;

Plane::Plane() {
	normal   = Vector3(0.0f, 1.0f, 0.0f);
	distance = 0.0f;
};

Plane::Plane(const Vector3 &normal, float distance, bool normalise) {
	this->normal = normal;
	this->distance = distance;

	if(normalise) {
		this->distance /= normal.Length();
		this->normal.Normalise();
	}
}

bool Plane::SphereInPlane(const Vector3 &position, float radius) const {
	return Vector3::Dot(position, normal) + distance <= -radius;
}

bool Plane::PointInPlane(const Vector3 &position) const {
	return Vector3::Dot(position, normal) + distance < -0.001f;
}

Plane Plane::PlaneFromTri(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2) {
	Vector3 v1v0 = v1 - v0;
	Vector3 v2v0 = v2 - v0;

	Vector3 normal = Vector3::Cross(v1v0, v2v0);

	
	normal.Normalise();
	float d = -Vector3::Dot(v0, normal);
	return Plane(normal, d, false);
}

float Plane::DistanceFromPlane(const Vector3 &in) const{
	return Vector3::Dot(in, normal) + distance;
}

Vector3 Plane::ProjectPointOntoPlane(const Vector3 &point) const {
	float distance = DistanceFromPlane(point);

	return point - (normal * distance);
}
