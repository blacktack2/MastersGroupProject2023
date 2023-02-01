#pragma once
#include "GameObject.h"
#include "PushdownMachine.h"
#include "PushdownState.h"
#include "NavigationMap.h"

#include "Debug.h"

#include <vector>

namespace NCL {
	namespace CSC8503 {
		class EnemyState;
		class PlayerObject;

		class EnemyObject : public GameObject {
		public:
			EnemyObject(GameWorld& gameWorld, PlayerObject& player, NavigationMap& navMap);
			~EnemyObject();

			virtual void Update(float dt) override;

			void Daze();

			bool CanSeePlayer();
			bool IsDazed() {
				return lastHitByBullet < dazeDuration;
			}

			std::vector<Vector3> GetRandomPatrolPoint();

			void FollowPlayer(float speed);
			void MoveTo(const Vector3& position, float speed);

			float SquareDistanceFrom(const Vector3& position);
		private:
			PlayerObject& player;
			NavigationMap& navMap;

			PushdownMachine stateMachine;

			const float dazeDuration = 10.0f;
			float lastHitByBullet = dazeDuration;
		};

		class EnemyState : public PushdownState {
		public:
			EnemyState(EnemyObject& enemy) : enemy(enemy) {}
		protected:
			EnemyObject& enemy;
		};

		class DazedEnemyState : public EnemyState {
		public:
			DazedEnemyState(EnemyObject& enemy) : EnemyState(enemy) {}

			PushdownResult OnUpdate(float dt, PushdownState** newState) override {
				if (!enemy.IsDazed()) {
					return PushdownResult::Pop;
				}
				return PushdownResult::NoChange;
			}

			void OnAwake() override {
				enemy.Daze();
				std::cout << "A goose is dazed!\n";
			}
		};

		class ChaseEnemyState : public EnemyState {
		public:
			ChaseEnemyState(EnemyObject& enemy) : EnemyState(enemy) {}

			PushdownResult OnUpdate(float dt, PushdownState** newState) override {
				if (enemy.IsDazed()) {
					*newState = new DazedEnemyState(enemy);
					return PushdownResult::Push;
				}
				if (!enemy.CanSeePlayer()) {
					return PushdownResult::Pop;
				}
				enemy.FollowPlayer(chaseSpeed);
				return PushdownResult::NoChange;
			}

			void OnAwake() override {
				std::cout << "A goose is chasing!\n";
			}
		private:
			const float chaseSpeed = 20.0f;
		};

		class PatrolEnemyState : public EnemyState {
		public:
			PatrolEnemyState(EnemyObject& enemy) : EnemyState(enemy) {}

			PushdownResult OnUpdate(float dt, PushdownState** newState) override {
				if (enemy.IsDazed()) {
					*newState = new DazedEnemyState(enemy);
					return PushdownResult::Push;
				}
				if (enemy.CanSeePlayer()) {
					*newState = new ChaseEnemyState(enemy);
					return PushdownResult::Push;
				}
				if (waypoints.empty()) {
					waypoints = enemy.GetRandomPatrolPoint();
					nextPatrolPoint = waypoints.empty() ? Vector3() : waypoints[0];
				} else if (enemy.SquareDistanceFrom(nextPatrolPoint) > 1.0f) {
					enemy.MoveTo(nextPatrolPoint, patrolSpeed);
				} else {
					waypoints.erase(waypoints.begin());
					nextPatrolPoint = waypoints.empty() ? Vector3() : waypoints[0];
				}
				Vector3 pos = nextPatrolPoint;
				for (int i = 1; i < waypoints.size(); i++) {
					Debug::DrawLine(pos, waypoints[i], Vector4(0, 1, 0, 1));
					pos = waypoints[i];
				}
				return PushdownResult::NoChange;
			}

			void OnAwake() override {
				waypoints = enemy.GetRandomPatrolPoint();
				nextPatrolPoint = waypoints.empty() ? Vector3() : waypoints[0];
				std::cout << "A goose is patrolling!\n";
			}
		private:
			Vector3 nextPatrolPoint;
			std::vector<Vector3> waypoints;

			const float patrolSpeed = 10.0f;
		};
	}
}