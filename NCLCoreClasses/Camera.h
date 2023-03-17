/**
 * @file   Camera.h
 * @brief  Main camera class for handling 
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include "Vector3.h"
#include "../CSC8503CoreClasses/Transform.h"
#include "../CSC8503/Hud.h"

#include <memory>

using namespace NCL::CSC8503;

namespace NCL {
	namespace Maths {
		class Matrix4;
	}
	using namespace NCL::Maths;

	enum class CameraType {
		Orthographic,
		Perspective
	};

	class Camera {
	public:
		Camera();
		Camera(float pitch, float yaw, const Vector3& position);
		~Camera() = default;

		inline void SetFollow(Transform* transform) {
			follow = transform;
		}

		void UpdateCamera(float dt);


		

		inline int GetPlayerID() {
			return playerID;
		}

		inline void SetPlayerID(int playerID) {
			this->playerID = playerID;
		}

		inline float GetFieldOfVision() const {
			return fov;
		}

		inline float GetNearPlane() const {
			return nearPlane;
		}
		inline Camera& SetNearPlane(float val) {
			nearPlane = val;
			return *this;
		}

		inline float GetFarPlane() const {
			return farPlane;
		}
		inline Camera& SetFarPlane(float val) {
			farPlane = val;
			return *this;
		}

		/**
		 * @return View matrix based on the current camera variables. Suitable
		 * for sending directly to a shader.
		 */
		Matrix4 BuildViewMatrix() const;
		/**
		 * @return Projection matrix based on the current camera variables.
		 * Suitable for sending directly to a shader.
		 */
		Matrix4 BuildProjectionMatrix(float aspectRatio) const;

		/**
		 * @return World space position of the camera.
		 */
		inline Vector3 GetPosition() const {
			return position;
		}
		/**
		 * @brief Set the world space position of the camera. 
		 */
		inline Camera& SetPosition(const Vector3& val) {
			position = val;
			return *this;
		}

		/**
		 * @return Yaw rotation component (in degrees).
		 */
		inline float GetYaw() const {
			return yaw;
		}
		/**
		 * @param y Yaw rotation component (in degrees).
		 */
		inline Camera& SetYaw(float y) {
			yaw = y;
			return *this;
		}

		/**
		 * @return Pitch rotation component (in degrees).
		 */
		inline float GetPitch() const {
			return pitch;
		}
		/**
		 * @param p Pitch rotation component (in degrees).
		 */
		inline Camera& SetPitch(float p) {
			pitch = p;
			return *this;
		}

		inline Hud& GetHud() {
			return hud;
		}

		static Camera BuildPerspectiveCamera(const Vector3& pos, float pitch, float yaw, float fov, float near, float far);
		static Camera BuildOrthoCamera(const Vector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float near, float far);
	private:
		CameraType camType = CameraType::Perspective;

		float left      = 0.0f;
		float right     = 0.0f;
		float top       = 0.0f;
		float bottom    = 0.0f;
		float nearPlane = 1.0f;
		float farPlane  = 100.0f;
		float fov       = 45.0f;

		float yaw;
		float pitch;
		Vector3 position;

		Transform* follow    = nullptr;
		float followDistance = 10.0f;
		float followLat      = 0.0f;
		float followLon      = 0.0f;
		Vector3 lookat       = Vector3(1.0f, 0.0f, 0.0f);

		float smoothFactor = 50.0f;

		int playerID = 0;

		Hud hud{};
	};
}
