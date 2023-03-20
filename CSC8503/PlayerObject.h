#pragma once
#include "GameObject.h"
#include "SoundSource.h"
#include "InputKeyMap.h"
#include "GameStateManager.h"
#include "Health.h"
#include "PlayerBullet.h"
#include"AnimatedRenderObject.h"
namespace NCL {
	class Camera;
	class CapsuleVolume;
	namespace CSC8503 {
		class Bullet;

		enum PlayerMovingDirection {
			Idle,

			MoveForward,
			MoveBackward,
			MoveToLeft,
			MoveToRight,
			MoveForwardLeft,
			MoveForwardRight,
			MoveBackwardLeft,
			MoveBackwardRight,
		};

		class PlayerObject : public GameObject {
		public:
			PlayerObject(int playerID);
			~PlayerObject();

			void Update(float dt);

			void ClearCamera();

			virtual void ChangeLoseState();

			void CollisionWith(GameObject* other);

			void AttachCamera(int b) {
				hasCamera = b;
				//camera = cam;
			};

			Camera* GetCamera() {
				return camera;
			}

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

			void SetBoundingVolume(CapsuleVolume* vol);

		protected:
			void Movement(float dt);

			void Move(Vector3 dir);
			void MoveByPosition(float dt, Vector3 dir);
			void GetButtonInput(unsigned int keyPress);
			void GetAxisInput();
			void GetDir(Vector3& movingDir3D);

			void RotateYaw(float yaw);
			void RotateToCamera();
			void RotateByAxis();
			void RotatePlayer();
		public:
			void MoveCamera(float dt);
		protected:
			void CheckGround();

			PlayerBullet* PrepareBullet();
			virtual void Shoot();
			void Jump();
			void BulletModification(PlayerBullet* bullet){};

			int playerID = -1;

			//network
			bool isNetwork = false;

			//keymap
			NCL::InputKeyMap& keyMap;
			unsigned int lastKey;
			float axis[AxisInput::AxisInputDataMax];
			
			//gameplay
			Health health = Health(100);

			//shooting related
			float projectileForce = 15;
			const Vector3 projectileSpawnPoint = Vector3(0.0f, 0.0f, -1.0f);
			float projectileLifespan = 5.0f;
			float projectileFireRate = 0.1f;
			float projectileFireRateTimer = 0;

			float pitch = 0.0f;
			float yaw = 0.0f;
			
			//camera related
			Vector3 lookingAt = Vector3(0);

			void MoveAnimation();
		private:

			void SetupAudio();

			//jump related 
			bool onGround = false;
			float jumpTriggerDist = 0.01f;
			float jumpTimer = 0.0f;
			const float jumpCooldown = 0.5f;
			float jumpSpeed = 10.0f;
			float radius = 0;

			//movement related
			float moveSpeed = 0.4f;
			Vector3 lastDir = Vector3(0,0,0);
			PlayerMovingDirection playerMovingDirection = PlayerMovingDirection::Idle;
			
			//camera related
			Camera* camera;
			int hasCamera = 0;		// 0 means no camera; 1,2,3,4 indicates which playerCamera it is attached to.
			bool isFreeLook = false;
			float camTurnSpeed = 0.5f;

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
