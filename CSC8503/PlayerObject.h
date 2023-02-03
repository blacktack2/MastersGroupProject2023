#pragma once
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet;

		class PlayerObject : public GameObject {
		public:
			PlayerObject(int id, int& scoreCounter);
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

			void MoveCamera();

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
			unsigned int lastKey;

			//camera related
			bool hasCamera = false;
			bool isFreeLook = false;
			float camTurnSpeed = 0.5f;
			Vector3 cameraOffset = Vector3(0.5f, 5.0f, 2.0f);

			//shooting related
			float projectileForce = 10;
			const Vector3 projectileSpawnPoint = Vector3(0.0f, 0.9f, -1.0f);
			float projectileLifespan = 5.0f;
			float projectileFireRate = 0.1f;
			float projectileFireRateTimer = 0;

			//network
			bool networkPlayer = false;

			// legacy variables
			int& scoreCounter;

			std::set<int> collidedWith;

			float lastGoosed = 0.0f;
			const float gooseDelay = 2.0f;
		};
	}
}
