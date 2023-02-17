
/*********************************************************************
 * Author: Xiaoyang Liu
 * Date:   February 2023
 *********************************************************************/

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

namespace NCL
{
    namespace CSC8503
    {
        class Boss : public GameObject
        {

        public:
            Boss(GameGrid& gg)
                : GameObject()
            {
                gameGrid = &gg;
            }

            //~Boss(){}

            virtual void Update(float dt) override;

            void Chase(float speed, Vector3 destination, GameGrid* gameGrid, float dt);

            BossBullet* releaseBossBullet(Vector3 v, Vector3 s);

            bool RandomWalk();

            bool StabPlayer(PlayerObject* player);

            bool Spin(PlayerObject* player);

            bool UseLaserOnPlayer(PlayerObject* player);

            bool JumpTo(PlayerObject* player);

            bool JumpAway(PlayerObject* player);

            bool SeekHeal(bool& hasHeal);

            bool InkSea();

            bool BulletsStorm();

            Health* GetHealth()
            {
                return &health;
            }

            std::vector<BossBullet*> GetBossBulletsReleasedByBoss()
            {
                return bombsReleased;
            }

            void clearBossBulletList()
            {
                bombsReleased.clear();
            }

            bool isUsingInkSea()
            {
                return usingInkSea;
            }

        protected:

            // Housekeepings:
            GameGrid* gameGrid = nullptr;
            float deltaTime = 0.0f;

            std::vector<BossBullet*> bombsReleased;

            // Boss' attributes:
            Health health = Health(100);

            //game state
            GameStateManager* gameStateManager = &GameStateManager::instance();

            // Parameters for boss actions:
            Vector3 randomWalkDirection{ 1,0,1 };
            float randomWalkTimer = 9999.9f;

            float stabTimer = 9999.9f;

            float laserTimer = 9999.9f;

            float jumpToTimer = 0.0f;

            float jumpAwayTimer = 0.0f;

            float bulletsStormTimer = 0.0f;
            float bulletsStormFrequencyTimer = 9999.9f;
            float bulletsStormAngle = 0.0f;

            float inkSeaTimer = 0.0f;
            bool usingInkSea = false;
        };

        class BossBehaviorTree
        {
        public:
            BossBehaviorTree(Boss* boss, PlayerObject* player)
            {
                float offensiveHealthLowerBound = 50;
                float bossVision = 80;
                float distanceToHaveCloseCombat = 40;

                behaviorLock = new BehaviorLock(boss, player);

                root = new SelectorNode();

                SequenceNode* offensiveMode = new SequenceNode();
                root->addChild(offensiveMode);

                CheckBossHealthNode* checkHealthToOffense = new CheckBossHealthNode(boss, offensiveHealthLowerBound, 100);
                offensiveMode->addChild(checkHealthToOffense);

                SelectorNode* offensiveMoves = new SelectorNode();
                offensiveMode->addChild(offensiveMoves);

                SequenceNode* offensiveWhilePlayerAway = new SequenceNode();
                offensiveMoves->addChild(offensiveWhilePlayerAway);

                CheckPlayerDistanceToBossNode* checkPlayerDistanceToWander = new CheckPlayerDistanceToBossNode(boss, player, bossVision, 9999);
                offensiveWhilePlayerAway->addChild(checkPlayerDistanceToWander);

                WanderNode* randomWalk = new WanderNode();
                offensiveWhilePlayerAway->addChild(randomWalk);

                SequenceNode* offensiveWhilePlayerNear = new SequenceNode();
                offensiveMoves->addChild(offensiveWhilePlayerNear);

                CheckPlayerDistanceToBossNode* checkPlayerDistanceToAttack = new CheckPlayerDistanceToBossNode(boss, player, -1, bossVision);
                offensiveWhilePlayerNear->addChild(checkPlayerDistanceToAttack);

                SelectorNode* offensiveCloseOrRemote = new SelectorNode();
                offensiveWhilePlayerNear->addChild(offensiveCloseOrRemote);

                SequenceNode* offensiveCloseCombat = new SequenceNode();
                offensiveCloseOrRemote->addChild(offensiveCloseCombat);

                CheckPlayerDistanceToBossNode* checkPlayerDistanceForCloseCombat = new CheckPlayerDistanceToBossNode(boss, player, -1, distanceToHaveCloseCombat);
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

                CheckPlayerDistanceToBossNode* checkPlayerDistanceForRemoteCombat = new CheckPlayerDistanceToBossNode(boss, player, distanceToHaveCloseCombat, bossVision);
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

                CheckPlayerDistanceToBossNode* checkPlayerIsAway = new CheckPlayerDistanceToBossNode(boss, player, bossVision, 9999);
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

                CheckPlayerDistanceToBossNode* checkPlayerIsNear = new CheckPlayerDistanceToBossNode(boss, player, -1, bossVision);
                defensiveWhilePlayerNear->addChild(checkPlayerIsNear);

                SelectorNode* defensiveCloseOrRemote = new SelectorNode();
                defensiveWhilePlayerNear->addChild(defensiveCloseOrRemote);

                SequenceNode* defensiveCloseCombat = new SequenceNode();
                defensiveCloseOrRemote->addChild(defensiveCloseCombat);

                CheckPlayerDistanceToBossNode* checkPlayerDistanceForDanger = new CheckPlayerDistanceToBossNode(boss, player, -1, distanceToHaveCloseCombat);
                defensiveCloseCombat->addChild(checkPlayerDistanceForDanger);

                JumpAwayFromPlayerNode* jumpAwayFromPlayer = new JumpAwayFromPlayerNode();
                defensiveCloseCombat->addChild(jumpAwayFromPlayer);

                SequenceNode* defensiveRemoteCombat = new SequenceNode();
                defensiveCloseOrRemote->addChild(defensiveRemoteCombat);

                CheckPlayerDistanceToBossNode* checkPlayerDistanceForKillingMoves = new CheckPlayerDistanceToBossNode(boss, player, distanceToHaveCloseCombat, bossVision);
                defensiveRemoteCombat->addChild(checkPlayerDistanceForKillingMoves);

                SelectorNode* chooseDefensiveRemoteCombat = new SelectorNode();
                defensiveRemoteCombat->addChild(chooseDefensiveRemoteCombat);

                SequenceNode* inkSea = new SequenceNode();
                chooseDefensiveRemoteCombat->addChild(inkSea);

                RandomBivalentSelectorNode* possibilityToUseInkSea = new RandomBivalentSelectorNode(30);
                inkSea->addChild(possibilityToUseInkSea);

                UseInkSeaNode* useInkSea = new UseInkSeaNode();
                inkSea->addChild(useInkSea);

                SequenceNode* bulletsStorm = new SequenceNode();
                chooseDefensiveRemoteCombat->addChild(bulletsStorm);

                // if not using ink sea, then the boss must be using bullets storm, so we don't need any randomness here.

                UseBulletsStormNode* useBulletsStorm = new UseBulletsStormNode();
                bulletsStorm->addChild(useBulletsStorm);

            }

            void update()
            {
                if (behaviorLock->isLocked())
                {
                    behaviorLock->BossAct();
                }
                else
                {
                    root->execute(behaviorLock);
                }
            }

        private:

            enum BossAction
            {
                NoAction,
                Dead,
                RandomWalk,
                Stab,
                Spin,
                Laser,
                JumpTo,
                JumpAway,
                SeekHeal,
                InkSea,
                BulletsStorm
            };

            class BehaviorLock
            {
            public:
                BehaviorLock(Boss* b, PlayerObject* p)
                {
                    boss = b;
                    player = p;
                }

                void SetBossAction(BossAction b)
                {
                    bossAction = b;
                }

                void BossAct()
                {
                    bool finish = false;

                    bool hasHealKit = false;

                    switch (bossAction)
                    {
                    case NoAction:
                        //std::cout << "Error: Boss' behavior is locked while there is currently no action to perform!\n";
                        break;
                    case Dead:
                       // std::cout << "The boss has dead, and it should do nothing.\n";
                        // Note that, for current implementation, once the boss has dead, its action remain in Dead forever.
                        break;
                    case RandomWalk:
                       // std::cout << "Boss is walking randomly.\n";
                        finish = boss->RandomWalk();
                        break;
                    case Stab:
                        //std::cout << "Boss stabs the player.\n";
                        finish = boss->StabPlayer(player);
                        break;
                    case Spin:
                        //std::cout << "Boss is spining.\n";
                        finish = boss->Spin(player);
                        break;
                    case Laser:
                        //std::cout << "Boss use laser.\n";
                        finish = boss->UseLaserOnPlayer(player);
                        break;
                    case JumpTo:
                        //std::cout << "Boss jumps towards the player.\n";
                        finish = boss->JumpTo(player);
                        break;
                    case JumpAway:
                        //std::cout << "Boss jumps away from the player.\n";
                        finish = boss->JumpAway(player);
                        break;
                    case SeekHeal:
                        //std::cout << "Boss is seeking for healing kit.\n";
                        finish = boss->SeekHeal(hasHealKit);
                        if (!hasHealKit)
                        {
                            bossAction = RandomWalk;
                        }
                        break;
                    case InkSea:
                        //std::cout << "Boss perfroms Ink Sea.\n";
                        finish = boss->InkSea();
                        break;
                    case BulletsStorm:
                        //std::cout << "Boss perfroms Bullets Storm.\n";
                        finish = boss->BulletsStorm();
                        break;
                    default:
                        std::cout << "Error: Boss' action is not a valid enum!\n";
                        break;
                    }

                    if (finish)
                    {
                        bossAction = NoAction;
                    }
                }

                bool isLocked()
                {
                    if (bossAction == NoAction)
                    {
                        return false;
                    }
                    return true;
                }

            private:
                BossAction bossAction = NoAction;
                Boss* boss;
                PlayerObject* player;
            };


            class Node
            {
            public:
                virtual ~Node() {}
                virtual bool execute(BehaviorLock* lock) = 0;
            protected:

            };

            class SelectorNode : public Node
            {
            public:
                void addChild(Node* child)
                {
                    children.push_back(child);
                }

                virtual bool execute(BehaviorLock* lock)
                {
                    for (Node* child : children)
                    {
                        if (child->execute(lock))
                        {
                            return true;
                        }
                    }

                    return false;
                }

            private:
                std::vector<Node*> children;
            };

            class RandomBivalentSelectorNode : public Node
            {
            public:
                RandomBivalentSelectorNode(int f)
                {
                    firstCasePercentage = f;
                }
                virtual bool execute(BehaviorLock* lock)
                {
                    int sample = std::rand() % 100;
                    if (sample < firstCasePercentage)
                    {
                        return true;
                    }
                    return false;
                }
            protected:
                int firstCasePercentage;
            };

            class SequenceNode : public Node
            {
            public:
                void addChild(Node* child)
                {
                    children.push_back(child);
                }

                virtual bool execute(BehaviorLock* lock)
                {
                    for (Node* child : children)
                    {
                        if (!child->execute(lock))
                        {
                            return false;
                        }
                    }

                    return true;
                }

            private:
                std::vector<Node*> children;
            };

            class CheckBossHealthNode : public Node
            {
            public:
                CheckBossHealthNode(Boss* b, float l, float u)
                {
                    boss = b;
                    lowerLimit = l;
                    upperLimit = u;
                }
                virtual bool execute(BehaviorLock* lock)
                {
                    if ((lowerLimit < boss->GetHealth()->GetHealth()) && (boss->GetHealth()->GetHealth() <= upperLimit))
                    {
                        return true;
                    }
                    return false;
                }
            protected:
                float lowerLimit;
                float upperLimit;
                Boss* boss = nullptr;
            };

            class CheckPlayerDistanceToBossNode : public Node
            {
            public:
                CheckPlayerDistanceToBossNode(Boss* b, PlayerObject* p, float l, float u)
                {
                    lowerLimit = l;
                    upperLimit = u;
                    boss = b;
                    player = p;
                }
                virtual bool execute(BehaviorLock* lock)
                {
                    float d = (boss->GetTransform().GetGlobalPosition() - player->GetTransform().GetGlobalPosition()).Length();
                    if ((lowerLimit < d) && (d <= upperLimit))
                    {
                        return true;
                    }
                    return false;
                }
            protected:
                float lowerLimit;
                float upperLimit;
                Boss* boss;
                PlayerObject* player;
            };

            class StabPlayerNode : public Node
            {
            public:
                virtual bool execute(BehaviorLock* lock)
                {
                    lock->SetBossAction(Stab);
                    return true;
                }
            };

            class SpinOnPlayerNode : public Node
            {
            public:
                virtual bool execute(BehaviorLock* lock)
                {
                    lock->SetBossAction(Spin);
                    return true;
                }
            };

            class UseLaserOnPlayerNode : public Node
            {
            public:
                virtual bool execute(BehaviorLock* lock)
                {
                    lock->SetBossAction(Laser);
                    return true;
                }
            };

            class JumpToPlayerNode : public Node
            {
            public:
                virtual bool execute(BehaviorLock* lock)
                {
                    lock->SetBossAction(JumpTo);
                    return true;
                }
            };

            class JumpAwayFromPlayerNode : public Node
            {
            public:
                virtual bool execute(BehaviorLock* lock)
                {
                    lock->SetBossAction(JumpAway);
                    return true;
                }
            };

            class WanderNode : public Node
            {
            public:
                virtual bool execute(BehaviorLock* lock)
                {
                    lock->SetBossAction(RandomWalk);
                    return true;
                }
            };

            class SeekForHealingNode : public Node
            {
            public:
                virtual bool execute(BehaviorLock* lock)
                {
                    lock->SetBossAction(SeekHeal);
                    return true;
                }
            };

            class UseBulletsStormNode : public Node
            {
            public:
                virtual bool execute(BehaviorLock* lock)
                {
                    lock->SetBossAction(BulletsStorm);
                    return true;
                }
            };

            class UseInkSeaNode : public Node
            {
            public:
                virtual bool execute(BehaviorLock* lock)
                {
                    lock->SetBossAction(InkSea);
                    return true;
                }
            };

            BehaviorLock* behaviorLock = nullptr;
            SelectorNode* root = nullptr;

        };
    }
}
