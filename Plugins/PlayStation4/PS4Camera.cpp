#include "PS4Camera.h"

NCL::PS4::PS4Camera::PS4Camera()
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
	if (mInput) { // remove once identified
		mInput->Poll();

		//Update based on right analog stick
		pitch -= (mInput->GetAxis(1).y);
		yaw -= (mInput->GetAxis(1).x);

		pitch = std::min(pitch, 90.0f);
		pitch = std::max(pitch, -90.0f);

		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}

		float frameSpeed = 32 * dt;

		float right = mInput->GetAxis(0).x;
		float forward = mInput->GetAxis(0).y;

		//right = std::min(right, 1.0f);
		//right = std::max(right, -1.0f);

		//forward = std::min(forward, 1.0f);
		//forward = std::max(forward, -1.0f);

		//std::cout << "Forward: " << forward << std::endl;
		//std::cout << "Right: " << right<< std::endl;

		if (mInput->GetAxis(2).y > 0 || forward < 0) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
		}
		if (mInput->GetAxis(2).x > 0 || right > 0) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(1, 0, 0) * frameSpeed;
		}

		if (mInput->GetAxis(2).y < 0 || forward>0) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
		}
		if (mInput->GetAxis(2).x < 0 || right < 0) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(1, 0, 0) * frameSpeed;
		}
	}
	

	//if () {
	//	position.y += frameSpeed;
	//}
	//if () {
	//	position.y -= frameSpeed;
	//}

}
