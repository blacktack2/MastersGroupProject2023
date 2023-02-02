#pragma once
#include "GameObject.h"


namespace NCL {
	namespace CSC8503 {
		class Bullet;

		class PlayerObject : public GameObject {
		public:
			PlayerObject(GameWorld& gameWorld, int id, int& scoreCounter);
			~PlayerObject();

			virtual void Update(float dt) override;

			void AddPoints(int points);

			void CollisionWith(GameObject* other);

			void AttachedCamera() {
				hasCamera = true;
			};

		protected:
			int id;

		private:
			void MoveTo(Vector3 position);
			void Move();
			void GetInput(Vector3& dir);

			void RotateYaw(float yaw);
			void RotateToCamera();

			void CheckGround();
			void Shoot();

			//jump related 
			bool onGround = false;
			float jumpTimer = 0.0f;
			const float jumpCooldown = 0.1f;
			float jumpSpeed = 10.0f;

			//movement related
			float moveSpeed = 0.4f;
			Vector3 lastDir = Vector3(0,0,0);

			//camera related
			bool hasCamera = false;
			bool isFreeLook = false;

			int& scoreCounter;

			std::set<int> collidedWith;

			float toungeMaxDistance = 10.0f;
			const Vector3 toungePos = Vector3(0.0f, 0.8f, -1.0f);
			Vector3 toungeContactPoint = Vector3(0);

			const Vector3 eyePosL = Vector3(-0.1f, 0.9f, -1.0f);
			const Vector3 eyePosR = Vector3( 0.1f, 0.9f, -1.0f);

			Vector3 laserForce = Vector3(0, 0, -1000);
			float laserFireRate = 0.05f;
			float laserDelay = 0.0f;
			float laserLifespan = 1.0f;

			float lastGoosed = 0.0f;
			const float gooseDelay = 2.0f;
		};
	}
}
