/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "Matrix4.h"
#include "../CSC8503CoreClasses/Transform.h"
#include "Vector3.h"

namespace NCL {
	using namespace Maths;
	using namespace CSC8503;

	enum class CameraType {
		Orthographic,
		Perspective
	};

	enum ControlType
	{
		None,

		Controller_1,
		Controller_2,
		Controller_3,
		Controller_4,

		KeyboardMouse

	};

	class Camera {
	public:
		Camera(void);

		Camera(float pitch, float yaw, const Vector3& position);

		~Camera(void) = default;

		void SetFollow(Transform* transform, bool isSmooth = false);

		void UpdateCamera(float dt);

		ControlType GetControlType()
		{
			return controlledBy;
		}

		void SetControlType(ControlType type)
		{
			controlledBy = type;
		}

		float GetFieldOfVision() const {
			return fov;
		}

		float GetNearPlane() const {
			return nearPlane;
		}

		float GetFarPlane() const {
			return farPlane;
		}

		Camera& SetNearPlane(float val) {
			nearPlane = val;
			return *this;
		}
		
		Camera& SetFarPlane(float val) {
			farPlane = val;
			return *this;
		}

		//Builds a view matrix for the current camera variables, suitable for sending straight
		//to a vertex shader (i.e it's already an 'inverse camera matrix').
		Matrix4 BuildViewMatrix() const;

		Matrix4 BuildProjectionMatrix(float aspectRatio = 1.0f) const;

		//Gets position in world space
		Vector3 GetPosition() const { return position; }
		//Sets position in world space
		Camera&	SetPosition(const Vector3& val) { position = val;  return *this; }

		//Gets yaw, in degrees
		float	GetYaw()   const { return yaw; }
		//Sets yaw, in degrees
		Camera&	SetYaw(float y) { yaw = y;  return *this; }

		//Gets pitch, in degrees
		float	GetPitch() const { return pitch; }
		//Sets pitch, in degrees
		Camera& SetPitch(float p) { pitch = p; return *this; }

		static Camera BuildPerspectiveCamera(const Vector3& pos, float pitch, float yaw, float fov, float near, float far);
		static Camera BuildOrthoCamera(const Vector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float near, float far);

		//smoothing
		bool isSmooth;
		Vector3 LastPos;
		float smoothFactor = 60.0f;
	protected:
		CameraType camType;

		float	nearPlane;
		float	farPlane;
		float	left;
		float	right;
		float	top;
		float	bottom;

		float	fov;
		float	yaw;
		float	pitch;
		Vector3 position;

		Transform* follow = nullptr;
		float followDistance;
		float followLat;
		float followLon;
		Vector3 lookat;

		ControlType controlledBy = KeyboardMouse;
	};
}