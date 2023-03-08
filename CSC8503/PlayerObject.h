#pragma once
#include "GameObject.h"
#include "SoundSource.h"
#include "InputKeyMap.h"
#include "GameStateManager.h"
#include "Health.h"
#include "PlayerBullet.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet;

		class PlayerObject : public GameObject {
		public:
			PlayerObject(int playerID);
			~PlayerObject();

			void Update(float dt);

			virtual void ChangeLoseState();

			void CollisionWith(GameObject* other);

			void AttachedCamera(int b) {
				hasCamera = b;
				//camera = cam;
			};

			const std::vector<GameObject*> GetLastInstancedObjects() {
				return lastInstancedObjects;
			}

			Health* GetHealth() {
				return &health;
			}

			int GetPlayerID()
			{
				return playerID;
			}

		protected:
			void MoveTo(Vector3 position);
			void Move(Vector3 dir);
			void MoveByPosition(float dt, Vector3 dir);
			void GetInput(Vector3& dir, unsigned int keyPress = InputType::Empty);
			void GetControllerInput(Vector3& movingDir3D);

			void RotateYaw(float yaw);
			void RotateToCamera();
			void RotateByAxis();

			void MoveCamera();

			void CheckGround();

			PlayerBullet* PrepareBullet();
			virtual void Shoot();
			void BulletModification(PlayerBullet* bullet){};

			int playerID = -1;

			//network
			bool isNetwork = false;

			//keymap
			paintHell::InputKeyMap& keyMap;
			unsigned int lastKey;
			
			//gameplay
			Health health = Health(100);

			//shooting related
			float projectileForce = 10;
			const Vector3 projectileSpawnPoint = Vector3(0.0f, 0.9f, -1.0f);
			float projectileLifespan = 5.0f;
			float projectileFireRate = 0.1f;
			float projectileFireRateTimer = 0;

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
			int hasCamera = 0;		// 0 means no camera; 1,2,3,4 indicates which playerCamera it is attached to.
			bool isFreeLook = false;
			float camTurnSpeed = 0.5f;
			Vector3 cameraOffset = Vector3(0.5f, 5.0f, 2.0f);

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
