#pragma once
#include "GameObject.h"
#include "SoundSource.h"
#include "InputKeyMap.h"
#include "GameStateManager.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet;

		class PlayerObject : public GameObject {
		public:
			PlayerObject(int id);
			~PlayerObject();

			void Update(float dt);

			void CollisionWith(GameObject* other);

			void AttachedCamera() {
				hasCamera = true;
				//camera = cam;
			};

			const std::vector<GameObject*> GetLastInstancedObjects() {
				return lastInstancedObjects;
			}

			const float GetHealth() {
				return health;
			}
			void Damage(float damage) {
				health -= damage;
			}
			void Heal(float heal) {
				health += heal;
			}
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
			
			//gameplay
			float health = 100.0f;
			float damageOverTimeTimer = 0.0f;

		private:

			void SetupAudio();

			//jump related 
			bool onGround = false;
			float jumpTriggerDist = 1.1f;
			float jumpTimer = 0.0f;
			const float jumpCooldown = 0.005f;
			float jumpSpeed = 10.0f;

			//movement related
			float moveSpeed = 0.4f;
			Vector3 lastDir = Vector3(0,0,0);
			
			//camera related
			//Camera& camera;
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

			//instantiated objs
			std::vector<GameObject*> lastInstancedObjects;

			// legacy variables
			std::set<int> collidedWith;

			//sound
			SoundSource* playerSource;
			SoundSource* attackSource;

			//game state
			GameStateManager* gameStateManager = &GameStateManager::instance();
		};
	}
}
