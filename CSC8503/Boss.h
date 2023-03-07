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


class BehaviourNodeWithChildren;

namespace NCL::CSC8503 {
    class Boss : public GameObject {
    public:
        Boss();
        ~Boss();

        virtual void Update(float dt) override;

        virtual void ChangeLoseState();

        void Chase(float speed, Vector3 destination,GameGrid* grid, float dt);

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
        void SetTarget(PlayerObject* player) {
            target = player;
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
                
        Health* GetHealth()
        {
            return &health;
        }
    protected:

        void BuildTree();
        float SqrDistToTarget();
        float DistToTarget();

        float deltaTime = 0.0f;

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

        BehaviourNodeWithChildren* behaviourTree;
    };

    class BossBehaviorTree {
    public:
        BossBehaviorTree(Boss* boss) {
            this->boss = boss;
            float offensiveHealthLowerBound = 50;
            float bossVision = 80;
            float distanceToHaveCloseCombat = 40;

            behaviorLock = new BehaviorLock(boss, &target);

            root = new SelectorNode();

            SequenceNode* offensiveMode = new SequenceNode();
            root->addChild(offensiveMode);

            CheckBossHealthNode* checkHealthToOffense = new CheckBossHealthNode(boss, offensiveHealthLowerBound, 100);
            offensiveMode->addChild(checkHealthToOffense);

            SelectorNode* offensiveMoves = new SelectorNode();
            offensiveMode->addChild(offensiveMoves);

            SequenceNode* offensiveWhilePlayerAway = new SequenceNode();
            offensiveMoves->addChild(offensiveWhilePlayerAway);

            CheckPlayerDistanceToBossNode* checkPlayerDistanceToWander = new CheckPlayerDistanceToBossNode(boss, &target, bossVision, 9999);
            offensiveWhilePlayerAway->addChild(checkPlayerDistanceToWander);

            WanderNode* randomWalk = new WanderNode();
            offensiveWhilePlayerAway->addChild(randomWalk);

            SequenceNode* offensiveWhilePlayerNear = new SequenceNode();
            offensiveMoves->addChild(offensiveWhilePlayerNear);

            CheckPlayerDistanceToBossNode* checkPlayerDistanceToAttack = new CheckPlayerDistanceToBossNode(boss, &target, -1, bossVision);
            offensiveWhilePlayerNear->addChild(checkPlayerDistanceToAttack);

            SelectorNode* offensiveCloseOrRemote = new SelectorNode();
            offensiveWhilePlayerNear->addChild(offensiveCloseOrRemote);

            SequenceNode* offensiveCloseCombat = new SequenceNode();
            offensiveCloseOrRemote->addChild(offensiveCloseCombat);

            CheckPlayerDistanceToBossNode* checkPlayerDistanceForCloseCombat = new CheckPlayerDistanceToBossNode(boss, &target, -1, distanceToHaveCloseCombat);
            offensiveCloseCombat->addChild(checkPlayerDistanceForCloseCombat);

            SelectorNode* chooseOffensiveCloseCombat = new SelectorNode();
            offensiveCloseCombat->addChild(chooseOffensiveCloseCombat);

            SequenceNode* stab = new SequenceNode();
            chooseOffensiveCloseCombat->addChild(stab);

            RandomBivalentSelectorNode* possibilityToStab = new RandomBivalentSelectorNode(60);
            stab->addChild(possibilityToStab);

            StabPlayerNode* stabPlayer = new StabPlayerNode();
            stab->addChild(stabPlayer);

            SequenceNode* spin = new SequenceNode();
            chooseOffensiveCloseCombat->addChild(spin);

            // if not stab, then the boss must spin, so we don't need any randomness here.

            SpinOnPlayerNode* spinOnPlayer = new SpinOnPlayerNode();
            spin->addChild(spinOnPlayer);

            SequenceNode* offensiveRemoteCombat = new SequenceNode();
            offensiveCloseOrRemote->addChild(offensiveRemoteCombat);

            CheckPlayerDistanceToBossNode* checkPlayerDistanceForRemoteCombat = new CheckPlayerDistanceToBossNode(boss, &target, distanceToHaveCloseCombat, bossVision);
            offensiveRemoteCombat->addChild(checkPlayerDistanceForRemoteCombat);

            SelectorNode* chooseOffensiveRemoteCombat = new SelectorNode();
            offensiveRemoteCombat->addChild(chooseOffensiveRemoteCombat);

            SequenceNode* laser = new SequenceNode();
            chooseOffensiveRemoteCombat->addChild(laser);

            RandomBivalentSelectorNode* possibilityToUseLaser = new RandomBivalentSelectorNode(70);
            laser->addChild(possibilityToUseLaser);

            UseLaserOnPlayerNode* useLaserOnPlayer = new UseLaserOnPlayerNode();
            laser->addChild(useLaserOnPlayer);

            SequenceNode* jumpToPlayer = new SequenceNode();
            chooseOffensiveRemoteCombat->addChild(jumpToPlayer);

            // if not using laser, then the boss must be jumping on the player, so we don't need any randomness here.

            JumpToPlayerNode* jumpToPlayerNode = new JumpToPlayerNode();
            jumpToPlayer->addChild(jumpToPlayerNode);

            SequenceNode* defensiveMode = new SequenceNode();
            root->addChild(defensiveMode);

            CheckBossHealthNode* checkHealthToDefense = new CheckBossHealthNode(boss, -1, offensiveHealthLowerBound);
            defensiveMode->addChild(checkHealthToDefense);

            SelectorNode* defensiveMoves = new SelectorNode();
            defensiveMode->addChild(defensiveMoves);

            SequenceNode* defensiveWhilePlayerAway = new SequenceNode();
            defensiveMoves->addChild(defensiveWhilePlayerAway);

            CheckPlayerDistanceToBossNode* checkPlayerIsAway = new CheckPlayerDistanceToBossNode(boss, &target, bossVision, 9999);
            defensiveWhilePlayerAway->addChild(checkPlayerIsAway);

            SelectorNode* defensiveHealOrWalk = new SelectorNode();
            defensiveWhilePlayerAway->addChild(defensiveHealOrWalk);

            SeekForHealingNode* seekForHealing = new SeekForHealingNode();
            defensiveHealOrWalk->addChild(seekForHealing);

            // if SeekForHealing fails:
            WanderNode* randomWalk2 = new WanderNode();     // ??? Do we really need a new WanderNode? Yes if WanderNode() takes argument(s) and we want a different wandering style here
            defensiveHealOrWalk->addChild(randomWalk2);

            SequenceNode* defensiveWhilePlayerNear = new SequenceNode();
            defensiveMoves->addChild(defensiveWhilePlayerNear);

            CheckPlayerDistanceToBossNode* checkPlayerIsNear = new CheckPlayerDistanceToBossNode(boss, &target, -1, bossVision);
            defensiveWhilePlayerNear->addChild(checkPlayerIsNear);

            SelectorNode* defensiveCloseOrRemote = new SelectorNode();
            defensiveWhilePlayerNear->addChild(defensiveCloseOrRemote);

            SequenceNode* defensiveCloseCombat = new SequenceNode();
            defensiveCloseOrRemote->addChild(defensiveCloseCombat);

            CheckPlayerDistanceToBossNode* checkPlayerDistanceForDanger = new CheckPlayerDistanceToBossNode(boss, &target, -1, distanceToHaveCloseCombat);
            defensiveCloseCombat->addChild(checkPlayerDistanceForDanger);

            JumpAwayFromPlayerNode* jumpAwayFromPlayer = new JumpAwayFromPlayerNode();
            defensiveCloseCombat->addChild(jumpAwayFromPlayer);

            SequenceNode* defensiveRemoteCombat = new SequenceNode();
            defensiveCloseOrRemote->addChild(defensiveRemoteCombat);

            CheckPlayerDistanceToBossNode* checkPlayerDistanceForKillingMoves = new CheckPlayerDistanceToBossNode(boss, &target, distanceToHaveCloseCombat, bossVision);
            defensiveRemoteCombat->addChild(checkPlayerDistanceForKillingMoves);

            SelectorNode* chooseDefensiveRemoteCombat = new SelectorNode();
            defensiveRemoteCombat->addChild(chooseDefensiveRemoteCombat);

            SequenceNode* inkRain = new SequenceNode();
            chooseDefensiveRemoteCombat->addChild(inkRain);

            RandomBivalentSelectorNode* possibilityToUseInkRain = new RandomBivalentSelectorNode(70);
            inkRain->addChild(possibilityToUseInkRain);

            UseInkRainNode* useInkRain = new UseInkRainNode();
            inkRain->addChild(useInkRain);

            SequenceNode* bulletsStorm = new SequenceNode();
            chooseDefensiveRemoteCombat->addChild(bulletsStorm);

            // if not using ink rain, then the boss must be using bullets storm, so we don't need any randomness here.

            UseBulletsStormNode* useBulletsStorm = new UseBulletsStormNode();
            bulletsStorm->addChild(useBulletsStorm);
        }

        ~BossBehaviorTree() {
            delete behaviorLock;
            delete root;
        }
        enum BossAction {
            NoAction,
            Dead,
            RandomWalk,
            Stab,
            Spin,
            Laser,
            JumpTo,
            JumpAway,
            SeekHeal,
            InkRain,
            BulletsStorm
        };

        void ChangeTarget(PlayerObject* target) {
            this->target = target;
        }

        BossAction GetBossAction() {
            return behaviorLock->GetBossAction();
        }
        void SetBossAction(BossAction action) {
            behaviorLock->SetBossAction(action);
        }

        void update() {
            //Debug::DrawLine(boss->GetTransform().GetGlobalPosition(), target->GetTransform().GetGlobalPosition(), Debug::BLUE, 0.01f);
            if (behaviorLock->isLocked()) {
                behaviorLock->BossAct();
            } else {
                root->execute(behaviorLock);
            }
        }
    private:

        class BehaviorLock {
        public:
            BehaviorLock(Boss* b, PlayerObject** p) {
                boss = b;
                player = p;
            }

            BossAction GetBossAction() {
                return bossAction;
            }
            void SetBossAction(BossAction b) {
                bossAction = b;
            }

            void BossAct() {
                bool finish = false;

                bool hasHealKit = false;

                // Orientation Correction:
                if (bossAction != NoAction && bossAction != Dead && bossAction != RandomWalk && bossAction != SeekHeal && (*player))
                {
                    Quaternion orientation = Quaternion(Matrix4::BuildViewMatrix((*player) ->GetTransform().GetGlobalPosition(), boss->GetTransform().GetGlobalPosition(), Vector3(0, 1, 0)).Inverse());
                    boss->GetTransform().SetOrientation(orientation);
                }


                switch (bossAction) {
                case NoAction:
                    //std::cout << "Error: Boss' behavior is locked while there is currently no action to perform!\n";
                    break;
                case Dead:
                    //std::cout << "The boss has dead, and it should do nothing.\n";
                    //Note that, for current implementation, once the boss has dead, its action remain in Dead forever.
                    break;
                case RandomWalk:
                    //std::cout << "Boss is walking randomly.\n";
                    finish = boss->RandomWalk();
                    break;
                case Stab:
                    //std::cout << "Boss stabs the player.\n";
                    finish = boss->StabPlayer(*player);
                    break;
                case Spin:
                    //std::cout << "Boss is spining.\n";
                    finish = boss->Spin(*player);
                    break;
                case Laser:
                    //std::cout << "Boss use laser.\n";
                    finish = boss->UseLaserOnPlayer(*player);
                    break;
                case JumpTo:
                    //std::cout << "Boss jumps towards the player.\n";
                    finish = boss->JumpTo(*player);
                    break;
                case JumpAway:
                    //std::cout << "Boss jumps away from the player.\n";
                    finish = boss->JumpAway(*player);
                    break;
                case SeekHeal:
                    //std::cout << "Boss is seeking for healing kit.\n";
                    finish = boss->SeekHeal(hasHealKit);
                    if (!hasHealKit) {
                        bossAction = RandomWalk;
                    }
                    break;
                case InkRain:
                    //std::cout << "Boss perfroms Ink Rain.\n";
                    finish = boss->InkRain(*player);
                    break;
                case BulletsStorm:
                    //std::cout << "Boss perfroms Bullets Storm.\n";
                    finish = boss->BulletsStorm();
                    break;
                default:
                    //std::cout << "Error: Boss' action is not a valid enum!\n";
                    break;
                }

                if (finish) {
                    bossAction = NoAction;
                }
            }

            bool isLocked() {
                if (bossAction == NoAction) {
                    return false;
                }
                return true;
            }

        private:
            BossAction bossAction = NoAction;
            Boss* boss;
            PlayerObject** player;
        };

            class Node {
            public:
                virtual ~Node() {}
                virtual bool execute(BehaviorLock* lock) = 0;
            protected:

            };

            class SelectorNode : public Node {
            public:

                ~SelectorNode() {
                    for (Node* node : children)
                    {
                        delete node;
                    }
                }

                void addChild(Node* child) {
                    children.push_back(child);
                }

                virtual bool execute(BehaviorLock* lock) {
                    for (Node* child : children) {
                        if (child->execute(lock)) {
                            return true;
                        }
                    }

                    return false;
                }
            private:
                std::vector<Node*> children;
            };

            class RandomBivalentSelectorNode : public Node {
            public:
                RandomBivalentSelectorNode(int f) {
                    firstCasePercentage = f;
                }

                ~RandomBivalentSelectorNode() {}
                virtual bool execute(BehaviorLock* lock) {
                    int sample = std::rand() % 100;
                    if (sample < firstCasePercentage) {
                        return true;
                    }
                    return false;
                }
            protected:
                int firstCasePercentage;
            };

            class SequenceNode : public Node {
            public:

                ~SequenceNode() {
                    for (Node* node : children)
                    {
                        delete node;
                    }
                }

                void addChild(Node* child) {
                    children.push_back(child);
                }

                virtual bool execute(BehaviorLock* lock) {
                    for (Node* child : children) {
                        if (!child->execute(lock)) {
                            return false;
                        }
                    }

                    return true;
                }

            private:
                std::vector<Node*> children;
            };

            class CheckBossHealthNode : public Node {
            public:
                CheckBossHealthNode(Boss* b, float l, float u) {
                    boss = b;
                    lowerLimit = l;
                    upperLimit = u;
                }

                ~CheckBossHealthNode() {}

                virtual bool execute(BehaviorLock* lock) {
                    if ((lowerLimit < boss->GetHealth()->GetHealth()) && (boss->GetHealth()->GetHealth() <= upperLimit)) {
                        return true;
                    }
                    return false;
                }
            protected:
                float lowerLimit;
                float upperLimit;
                Boss* boss = nullptr;
            };

        class CheckPlayerDistanceToBossNode : public Node {
        public:
            CheckPlayerDistanceToBossNode(Boss* b, PlayerObject** target, float l, float u) {
                lowerLimit = l;
                upperLimit = u;
                boss = b;
                this->target = target;
            }
            virtual bool execute(BehaviorLock* lock) {
                float d;
                if(*target)
                    d = (boss->GetTransform().GetGlobalPosition() - (* target)->GetTransform().GetGlobalPosition()).Length();
                else {
                    d = 100;
                }
                if ((lowerLimit < d) && (d <= upperLimit)) {
                    return true;
                }
                return false;
            }
        protected:
            float lowerLimit;
            float upperLimit;
            Boss* boss;
            PlayerObject** target;
        };

            class StabPlayerNode : public Node {
            public:

                StabPlayerNode() {}
                ~StabPlayerNode() {}

                virtual bool execute(BehaviorLock* lock) {
                    lock->SetBossAction(Stab);
                    return true;
                }
            };

            class SpinOnPlayerNode : public Node {
            public:
                SpinOnPlayerNode() {}
                ~SpinOnPlayerNode() {}

                virtual bool execute(BehaviorLock* lock) {
                    lock->SetBossAction(Spin);
                    return true;
                }
            };

            class UseLaserOnPlayerNode : public Node {
            public:
                UseLaserOnPlayerNode() {}
                ~UseLaserOnPlayerNode() {}

                virtual bool execute(BehaviorLock* lock) {
                    lock->SetBossAction(Laser);
                    return true;
                }
            };

            class JumpToPlayerNode : public Node {
            public:
                JumpToPlayerNode() {}
                ~JumpToPlayerNode() {}

                virtual bool execute(BehaviorLock* lock) {
                    lock->SetBossAction(JumpTo);
                    return true;
                }
            };

            class JumpAwayFromPlayerNode : public Node {
            public:
                JumpAwayFromPlayerNode() {}
                ~JumpAwayFromPlayerNode() {}

                virtual bool execute(BehaviorLock* lock) {
                    lock->SetBossAction(JumpAway);
                    return true;
                }
            };

            class WanderNode : public Node {
            public:
                WanderNode() {}
                ~WanderNode() {}

                virtual bool execute(BehaviorLock* lock) {
                    lock->SetBossAction(RandomWalk);
                    return true;
                }
            };

            class SeekForHealingNode : public Node {
            public:
                SeekForHealingNode() {}
                ~SeekForHealingNode() {}

                virtual bool execute(BehaviorLock* lock) {
                    lock->SetBossAction(SeekHeal);
                    return true;
                }
            };

            class UseBulletsStormNode : public Node {
            public:
                UseBulletsStormNode() {}
                ~UseBulletsStormNode() {}

                virtual bool execute(BehaviorLock* lock) {
                    lock->SetBossAction(BulletsStorm);
                    return true;
                }
            };

            class UseInkRainNode : public Node {
            public:
                UseInkRainNode() {}
                ~UseInkRainNode() {}
                virtual bool execute(BehaviorLock* lock) {
                    lock->SetBossAction(InkRain);
                    return true;
                }
            };

        BehaviorLock* behaviorLock = nullptr;
        SelectorNode* root = nullptr;

        PlayerObject* target;
        Boss* boss;
    };
}
