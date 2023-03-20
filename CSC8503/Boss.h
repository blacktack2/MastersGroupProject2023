/**
 * @file   Boss.h
 * @brief  
 * 
 * @author Xiaoyang Liu
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "GameObject.h"
#include "Debug.h"
#include "NavigationPath.h"
#include "GameGrid.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"
#include "GameWorld.h"

#include "GameGridManager.h"
#include "InkEffectManager.h"

#include "Obstacle.h"
#include "BossBullet.h"
#include "SphereVolume.h"

#include "AnimatedRenderObject.h"

#include "Vector3.h"


class BehaviourNodeWithChildren;

namespace NCL {
    namespace CSC8503 {
        class Boss : public GameObject {
        public:

            enum BossAction {
                NoAction,
                Dead,
                Move1, //Random Walk
                Attack1, //stab
                Attack2, //spin
                Attack3, //laser
                Move2, //jump to
                Move3, //jump away
                Attack4, //ink rain
                Attack5 //bulletStorm
            };


            Boss();
            ~Boss();

            virtual void Update(float dt) override;

            virtual void ChangeLoseState();

            void Chase(float speed, Vector3 destination, GameGrid* grid, float dt);

            BossBullet* releaseBossBullet(Vector3 v, Vector3 s, Vector3 p = Vector3{ 99999,99999,99999 });

            /**
             * @brief Allows further modification of the spawned bullet. Also provide a template for networked bullets.
             *
             * @param bullet The bullet being modified.
             */
            virtual void BulletModification(BossBullet* bullet) {};

            /**
              * @brief Allows Boss to aquire new target.
              */
            virtual void ChangeTarget();

            PlayerObject* GetTarget() {
                return target;
            }
            void SetNextTarget(PlayerObject* player) {
                if (target == nullptr) {
                    target = player;
                }
                nextTarget = player;
            }

            bool RandomWalk();

            bool StabPlayer(PlayerObject* player);

            bool Spin(PlayerObject* player);

            bool UseLaserOnPlayer(PlayerObject* player);

            bool JumpTo(PlayerObject* player);

            bool JumpAway(PlayerObject* player);

            bool SeekHeal(bool& hasHeal);

            bool InkRain(PlayerObject* player);

            bool BulletsStorm();

            BossAction GetBossAction() {
                return bossAction;
            }

            Health* GetHealth()
            {
                return &health;
            }
        protected:

            void BuildTree();
            float SqrDistToTarget();
            float DistToTarget();

            void SetBossOrientation(Vector3* facingDir);

            float deltaTime = 0.0f;
            bool isClient = false;

            Health health = Health(100);

            GameStateManager* gameStateManager = &GameStateManager::instance();

            Vector3 randomWalkDirection{ 1,0,1 };
            float randomWalkTimer = 9999.9f;

            float stabTimer = 9999.9f;

            float laserTimer = 9999.9f;

            float jumpToTimer = 0.0f;

            float jumpAwayTimer = 0.0f;

            float bulletsStormTimer = 0.0f;
            float bulletsStormFrequencyTimer = 9999.9f;
            float bulletsStormAngle = 0.0f;

            float inkRainTimer = 0.0f;
            bool rainIsInitialised = false;
            int currentRainBomb = 0;

            std::vector<BossBullet*> rain;
            std::vector<Vector3> rainBombPositions;

            PlayerObject* target;
            PlayerObject* nextTarget;

            BehaviourNodeWithChildren* behaviourTree;
            BossAction bossAction = NoAction;
        };
    }
}
