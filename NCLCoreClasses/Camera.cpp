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
#ifdef _ORBIS
#include "../Plugins/PlayStation4/PS4InputManager.h"
#endif // _ORBIS


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
	if (follow == nullptr) {
#ifdef x64
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);

		pitch = Maths::Clamp(pitch, -90.0f, 90.0f);
		yaw += (yaw < 0) ? 360.0f : ((yaw > 360.0f) ? -360.0f : 0.0f);

		float frameSpeed = (Window::GetKeyboard()->KeyDown(KeyboardKeys::CONTROL) ? 200 : 25) * dt;

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
			position += Matrix4::Rotation(yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * frameSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
			position -= Matrix4::Rotation(yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * frameSpeed;
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
			position += Matrix4::Rotation(yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * frameSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
			position -= Matrix4::Rotation(yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * frameSpeed;
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
			position.y -= frameSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
			position.y += frameSpeed;
		}
#endif // x64
#ifdef _ORBIS
		Maths::Vector2 leftAnalog = PS4::PS4InputManager::GetAxis(PS4::Player1, PS4::LEFTSTICK);
		Maths::Vector2 rightAnalog = PS4::PS4InputManager::GetAxis(PS4::Player1, PS4::RIGHTSTICK);
		Maths::Vector2 directionKeys = PS4::PS4InputManager::GetAxis(PS4::Player1, PS4::KEYPAD);
		//Update based on right analog stick
		pitch -= (rightAnalog.y);
		yaw -= (rightAnalog.x);

		pitch = NCL::Maths::(pitch, -90.0f, 90.0f);
		yaw += (yaw < 0) ? 360.0f : ((yaw > 360.0f) ? -360.0f : 0.0f);

		float frameSpeed = 32 * dt;

		//left analog stick
		float right = leftAnalog.x;
		float forward = leftAnalog.y;

		//right = std::min(right, 1.0f);
		//right = std::max(right, -1.0f);

		//forward = std::min(forward, 1.0f);
		//forward = std::max(forward, -1.0f);

		//std::cout << "Forward: " << forward << std::endl;
		//std::cout << "Right: " << right<< std::endl;

		if (directionKeys.y > 0 || forward < 0) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
		}
		if (directionKeys.x > 0 || right > 0) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(1, 0, 0) * frameSpeed;
		}

		if (directionKeys.y < 0 || forward>0) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
		}
		if (directionKeys.x < 0 || right < 0) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(1, 0, 0) * frameSpeed;
		}

		if (PS4::PS4InputManager::GetButtons(PS4::Player1, PS4::CROSS)) {
			position.y += frameSpeed;
		}
		if (PS4::PS4InputManager::GetButtons(PS4::Player1, PS4::SQUARE)) {
			position.y -= frameSpeed;
		}
#endif // _ORBIS

		
	} else {


		Vector3 followPos = follow->GetGlobalPosition();
#ifdef x64
		followDistance = Maths::Clamp(followDistance - (float)Window::GetMouse()->GetWheelMovement(), 5.0f, 40.0f);
#endif // x64
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
