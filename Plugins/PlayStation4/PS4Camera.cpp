#ifdef _ORBIS
#include "PS4Camera.h"
#include "PS4InputManager.h"
#include "Vector2.h"
#include "Maths.h"
#include "InputKeyMap.h"

NCL::PS4::PS4Camera::PS4Camera():Camera()
{
	nearPlane = 1.0f;
	farPlane = 1000.0f;
	pitch = -15.0f;
	yaw = 315.0f;
}

NCL::PS4::PS4Camera::PS4Camera(PS4Input* input):Camera()
{
	nearPlane = 1.0f;
	farPlane = 1000.0f;
	pitch = -15.0f;
	yaw = 315.0f;
	this->mInput = input;
}

void NCL::PS4::PS4Camera::UpdateCamera(float dt)
{
	if (follow == nullptr) {
		Maths::Vector2 leftAnalog = PS4InputManager::GetAxis(Player1, LEFTSTICK);
		Maths::Vector2 rightAnalog = PS4InputManager::GetAxis(Player1, RIGHTSTICK);
		Maths::Vector2 directionKeys = PS4InputManager::GetAxis(Player1, KEYPAD);
		//Update based on right analog stick
		pitch -= (rightAnalog.y);
		yaw -= (rightAnalog.x);

		pitch = Maths::Clamp(pitch, -90.0f, 90.0f);
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

		if (PS4::PS4InputManager::GetButtons(Player1, PS4::CROSS)) {
			position.y += frameSpeed;
		}
		if (PS4::PS4InputManager::GetButtons(Player1, PS4::SQUARE)) {
			position.y -= frameSpeed;
		}
		//if () {
		//	position.y += frameSpeed;
		//}
		//if () {
		//	position.y -= frameSpeed;
		//}
	}
	else {
		Vector3 followPos = follow->GetGlobalPosition();
		followPos = Vector3::Lerp(LastPos, followPos, std::min(smoothFactor * dt, 1.0f));
		LastPos = followPos;

		NCL::InputKeyMap& keyMap = NCL::InputKeyMap::instance();
		Vector2 orientationData{ 0,0 };
		if (keyMap.GetAxisData(playerID, AxisInput::Axis3, orientationData.x) && keyMap.GetAxisData(playerID, AxisInput::Axis4, orientationData.y))
		{
			if (!(orientationData.x == 0 && orientationData.y == 0))
			{
				float sensitivity = 1.5f;
				pitch += (orientationData.y * sensitivity);
				yaw -= (orientationData.x * sensitivity);
			}
		}

		pitch = std::clamp(pitch, -90.0f, 90.0f);
		yaw += (yaw < 0) ? 360.0f : ((yaw > 360.0f) ? -360.0f : 0.0f);


		//followDistance = Maths::Clamp(followDistance - (float)Window::GetMouse()->GetWheelMovement(), 5.0f, 40.0f);

		position = followPos - Vector3(std::cos(-NCL::Maths::DegreesToRadians(90.0f + yaw)), Maths::DegreesToRadians(pitch), std::sin(-Maths::DegreesToRadians(90.0f + yaw))) * followDistance;

	}
}
#endif //_ORBIS
