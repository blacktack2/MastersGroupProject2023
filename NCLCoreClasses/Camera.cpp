/**
 * @file   Camera.cpp
 * @brief  See Camera.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "Camera.h"

#include "Maths.h"
#include "Matrix4.h"

#include "Window.h"

#include <algorithm>

using namespace NCL;
using namespace NCL::CSC8503;
using namespace NCL::Maths;

Camera::Camera() :
pitch(0.0f), yaw(0.0f), position(0.0f) {
}

Camera::Camera(float pitch, float yaw, const Vector3& position) :
pitch(pitch), yaw(yaw), position(position) {
}

void Camera::UpdateCamera(float dt) {
	if (follow != nullptr) {
		Vector3 followPos = follow->GetGlobalPosition();

		followDistance = std::clamp(followDistance - (float)Window::GetMouse()->GetWheelMovement(), 5.0f, 40.0f);

		Vector3 newPos = followPos - Vector3(
			std::cos(-Maths::DegreesToRadians(90.0f + yaw)) * std::cos(Maths::DegreesToRadians(pitch)),
			std::sin( Maths::DegreesToRadians(pitch)),
			std::sin(-Maths::DegreesToRadians(90.0f + yaw)) * std::cos(Maths::DegreesToRadians(pitch))
		) * followDistance;
		newPos.y += 1.0f;
		position = Vector3::Lerp(position, newPos + follow->GetGlobalOrientation() * Vector3(1.0f, 0.0f, 0.0f) * 1.5f, std::min(smoothFactor * dt, 1.0f));
		position.y = std::max(position.y, 0.1f);
	}
}

Matrix4 Camera::BuildViewMatrix() const {
	return (Matrix4::Rotation(-pitch, Vector3(1.0f, 0.0f, 0.0f)) *
			Matrix4::Rotation(-yaw  , Vector3(0.0f, 1.0f, 0.0f))) *
			Matrix4::Translation(-position);
}

Matrix4 Camera::BuildProjectionMatrix(float currentAspect) const {
	switch (camType) {
		default:
		case CameraType::Perspective:
			return Matrix4::Perspective(nearPlane, farPlane, currentAspect, fov);
		case CameraType::Orthographic:
			return Matrix4::Orthographic(left, right, bottom, top, nearPlane, farPlane);
	}
}

Camera Camera::BuildPerspectiveCamera(const Vector3& pos, float pitch, float yaw, float fov, float nearPlane, float farPlane) {
	Camera c = Camera(pitch, yaw, pos);
	c.camType  = CameraType::Perspective;

	c.nearPlane = nearPlane;
	c.farPlane  = farPlane;
	c.fov       = fov;

	return c;
}
Camera Camera::BuildOrthoCamera(const Vector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float nearPlane, float farPlane) {
	Camera c = Camera(pitch, yaw, pos);
	c.camType  = CameraType::Orthographic;

	c.nearPlane = nearPlane;
	c.farPlane  = farPlane;
	c.left      = left;
	c.right     = right;
	c.top       = top;
	c.bottom    = bottom;

	return c;
}
