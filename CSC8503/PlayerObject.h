#pragma once
#include "GameObject.h"
#include "InputKeyMap.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet;

		class PlayerObject : public GameObject {
		public:
			PlayerObject(int id);
			~PlayerObject();

			void Update(float dt);

			void AddPoints(int points);

			void CollisionWith(GameObject* other);

			void AttachedCamera() {
				hasCamera = true;
			};

		protected:
			void MoveTo(Vector3 position);
			void Move(Vector3 dir);
			void GetInput(Vector3& dir, unsigned int keyPress = InputType::Empty);

			void RotateYaw(float yaw);
			void RotateToCamera();

			void MoveCamera();

			void CheckGround();
			void Shoot();

			int id;

			//network
			bool isNetwork = false;

			//keymap
			paintHell::InputKeyMap& keyMap;
			unsigned int lastKey;

		private:

			//jump related 
			bool onGround = false;
			float jumpTimer = 0.0f;
			const float jumpCooldown = 0.005f;
			float jumpSpeed = 10.0f;

			//movement related
			float moveSpeed = 0.4f;
			Vector3 lastDir = Vector3(0,0,0);
			
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

			// legacy variables

			std::set<int> collidedWith;

			float lastGoosed = 0.0f;
			const float gooseDelay = 2.0f;
		};
	}
}
