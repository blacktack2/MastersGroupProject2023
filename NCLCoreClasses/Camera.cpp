#include "Camera.h"
#include "Maths.h"
#include "Window.h"

#include <algorithm>

using namespace NCL;
using namespace CSC8503;

void Camera::SetFollow(Transform* transform) {
	follow = transform;
}

Camera::Camera() {
	left	= 0;
	right	= 0;
	top		= 0;
	bottom	= 0;

	pitch		= 0.0f;
	yaw			= 0.0f;

	fov			= 45.0f;
	nearPlane	= 1.0f;
	farPlane	= 100.0f;

	followDistance = 10.0f;
	followLat = 0.0f;
	followLon = 0.0f;
	lookat = Vector3(1, 0, 0);

	camType		= CameraType::Perspective;
}

Camera::Camera(float pitch, float yaw, const Vector3& position) : Camera() {
	this->pitch		= pitch;
	this->yaw		= yaw;
	this->position	= position;

	this->fov		= 45.0f;
	this->nearPlane = 1.0f;
	this->farPlane	= 100.0f;

	this->followDistance = 10.0f;
	this->followLat = 0.0f;
	this->followLon = 0.0f;
	this->lookat = Vector3(1, 0, 0);

	this->camType	= CameraType::Perspective;
}

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float dt) {
	if (follow == nullptr) {
		//Update the mouse by how much
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);

		pitch = std::clamp(pitch, -90.0f, 90.0f);
		yaw += (yaw < 0) ? 360.0f : ((yaw > 360.0f) ? -360.0f : 0.0f);

		float frameSpeed = (Window::GetKeyboard()->KeyDown(KeyboardKeys::CONTROL) ? 200 : 25) * dt;

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
			position.y -= frameSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
			position.y += frameSpeed;
		}
	} else {
		Vector3 followPos = follow->GetPosition();
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);

		pitch = std::clamp(pitch, -90.0f, 90.0f);
		yaw += (yaw < 0) ? 360.0f : ((yaw > 360.0f) ? -360.0f : 0.0f);

		followDistance = std::clamp(followDistance - (float)Window::GetMouse()->GetWheelMovement(), 5.0f, 40.0f);

		position = followPos - Vector3(std::cos(-Maths::DegreesToRadians(90.0f + yaw)), Maths::DegreesToRadians(pitch), std::sin(-Maths::DegreesToRadians(90.0f + yaw))) * followDistance;
	}
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix() const {
	return
		(Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0))) *
		Matrix4::Translation(-position);
};

Matrix4 Camera::BuildProjectionMatrix(float currentAspect) const {
	if (camType == CameraType::Orthographic) {
		return Matrix4::Orthographic(left, right, bottom, top, nearPlane, farPlane);
	}
	//else if (camType == CameraType::Perspective) {
		return Matrix4::Perspective(nearPlane, farPlane, currentAspect, fov);
	//}
}

Camera Camera::BuildPerspectiveCamera(const Vector3& pos, float pitch, float yaw, float fov, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Perspective;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane  = farPlane;

	c.fov		= fov;

	return c;
}
Camera Camera::BuildOrthoCamera(const Vector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Orthographic;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane	= farPlane;

	c.left		= left;
	c.right		= right;
	c.top		= top;
	c.bottom	= bottom;

	return c;
}