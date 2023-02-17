
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
#include "GameGrid.h"

namespace NCL
{
    namespace CSC8503
    {
        //class BossBehaviorTree;

        class Bomb : public GameObject
        {
        public:
            Bomb(Vector3 v)
                : GameObject()
            {
                velocity = v;
            }

            //~Bomb(){}

            virtual void Update(float dt) override
            {
                this->GetTransform().SetPosition(this->GetTransform().GetGlobalPosition() + velocity * dt);
                lifeTime -= dt;
                if (lifeTime < 0.0f)
                {
                    Delete();
                }
                if (this->GetTransform().GetGlobalPosition().y < 1.0f)
                {
                    GetTransform().SetPosition({ GetTransform().GetGlobalPosition().x, 1.0f, GetTransform().GetGlobalPosition().z });
                }
            }

        protected:
            Vector3 velocity{ 0,0,0 };
            float lifeTime = 5.0f;
        };

        class Boss : public GameObject
        {

        public:
            Boss(GameGrid& gg)
                : GameObject()
            {
                gameGrid = &gg;
            }

            //~Boss(){}

            virtual void Update(float dt) override
            {
                deltaTime = dt;

                if (this->GetTransform().GetGlobalPosition().y < 1.0f)		// please fix the physics system
                {
                    GetTransform().SetPosition({ GetTransform().GetGlobalPosition().x, 1.0f, GetTransform().GetGlobalPosition().z });
                }
            }

            void Chase(float speed, Vector3 destination, GameGrid* gameGrid, float dt)
            {
                if (gameGrid == nullptr)
                {
                    return;
                }

                float aiSpeed = speed;

                NavigationPath samples;
                gameGrid->FindCatmullRomPath(this->GetTransform().GetGlobalPosition(), destination, samples);
                Vector3 node;
                std::vector<Vector3> tempSteps;
                while (samples.PopWaypoint(node)) {
                    tempSteps.push_back(node);
                }
                if (tempSteps.size() <= 100) {  // enemy is too close and will just go straight to the destination
                    this->GetTransform().SetPosition(this->GetTransform().GetGlobalPosition() + ((destination - this->GetTransform().GetGlobalPosition()).Normalised() * (dt * aiSpeed)));
                }
                else {
                    Vector3 closest{ 9999,9999,9999 };
                    Vector3 forward;
                    for (int n = 0; n < tempSteps.size() - 1; n++) {
                        if ((closest - this->GetTransform().GetGlobalPosition()).Length() > (tempSteps[n] - this->GetTransform().GetGlobalPosition()).Length()) {
                            closest = tempSteps[n];
                            forward = tempSteps[n + 1];
                        }
                    }

                    if ((closest - this->GetTransform().GetGlobalPosition()).Length() > 0.1f) {
                        Vector3 dir = (closest - this->GetTransform().GetGlobalPosition()).Normalised();
                        this->GetPhysicsObject()->AddForce(dir * 10.0f);
                    }

                    Vector3 dir = (forward - closest).Normalised();
                    this->GetTransform().SetPosition(this->GetTransform().GetGlobalPosition() + dir * (dt * aiSpeed));


                }

                // Draw the path
                if (tempSteps.size() > 1) {
                    for (int i = 1; i < tempSteps.size(); i++) {
                        Vector3 a = tempSteps[i - 1];
                        Vector3 b = tempSteps[i];

                        Debug::DrawLine(a, b, Vector4{ 0,1,0,1 }, 0.0f);
                    }
                }
            }

            Bomb* releaseBomb(Vector3 v, Vector3 s)
            {
                Bomb* bomb = new Bomb(v);
                Vector3 position = this->GetTransform().GetGlobalPosition();
                bomb->GetTransform()
                    .SetPosition(position)
                    .SetScale(s);

                bombsReleased.push_back(bomb);

                return bomb;
            }

            bool RandomWalk()   // TODO: the boss should not go outside the boundaries
            {
                float speed = 20.0f;
                float period = 1.0f;    // change direction in period-seconds
                randomWalkTimer += deltaTime;
                if (randomWalkTimer > period)
                {
                    randomWalkTimer = 0.0f;
                    float x = (std::rand() % 11) - 5;
                    float z = (std::rand() % 11) - 5;
                    randomWalkDirection = Vector3{ x,0,z };
                    randomWalkDirection = randomWalkDirection.Normalised();
                    return true;
                }
                Debug::DrawLine(GetTransform().GetGlobalPosition(), GetTransform().GetGlobalPosition() + randomWalkDirection * 15, Vector4{ 0,1,0,1 }, 0.0f);
                GetTransform().SetPosition(GetTransform().GetGlobalPosition() + randomWalkDirection * speed * deltaTime);
                return false;
            }

            bool StabPlayer(PlayerObject* player)
            {
                Vector3 bombScale{ 0.75,0.75,0.75 };
                float bombSpeed = 40.0f;
                Vector3 bombDirection = (player->GetTransform().GetGlobalPosition() - this->GetTransform().GetGlobalPosition()).Normalised();
                Vector3 bombVelocity = bombDirection * bombSpeed;
                float period = 1.0f;
                int numOfbombs = 10;
                stabTimer += deltaTime;
                if (stabTimer > period)
                {
                    stabTimer = 0.0f;
                    const float PI = 3.1415926f;
                    float dTheta = PI / 64;
                    releaseBomb(bombVelocity, bombScale);
                    Vector3 y{ 0,1,0 };
                    Vector3 yDot = Maths::Vector3::Cross(Maths::Vector3::Cross(bombDirection, y), bombDirection).Normalised();
                    for (int i = 1; i < numOfbombs; i++)
                    {
                        Vector3 dir = (bombDirection * cos(dTheta * i) + yDot * sin(dTheta * i)).Normalised();
                        Vector3 vel = dir * bombSpeed;
                        releaseBomb(vel, bombScale);
                    }
                    return true;
                }
                return false;
            }

            bool Spin(PlayerObject* player)
            {
                Vector3 bombScale{ 0.75,0.75,0.75 };
                float bombSpeed = 40.0f;
                Vector3 bombDirection = (player->GetTransform().GetGlobalPosition() - this->GetTransform().GetGlobalPosition()).Normalised();
                Vector3 bombVelocity = bombDirection * bombSpeed;
                float period = 1.0f;
                int numOfbombs = 10;
                stabTimer += deltaTime;
                if (stabTimer > period)
                {
                    stabTimer = 0.0f;
                    const float PI = 3.1415926f;
                    float dTheta = PI / 64;
                    releaseBomb(bombVelocity, bombScale);
                    Vector3 y{ 0,1,0 };
                    Vector3 leftDir = Maths::Vector3::Cross(y, bombDirection);
                    Vector3 yDot = Maths::Vector3::Cross(Maths::Vector3::Cross(bombDirection, leftDir), bombDirection).Normalised();
                    Vector3 yDotDot = Maths::Vector3::Cross(Maths::Vector3::Cross(bombDirection, -leftDir), bombDirection).Normalised();
                    for (int i = 1; i < numOfbombs; i++)
                    {
                        Vector3 dir = (bombDirection * cos(dTheta * i) + yDot * sin(dTheta * i)).Normalised();
                        Vector3 vel = dir * bombSpeed;
                        releaseBomb(vel, bombScale);
                    }
                    for (int i = 1; i < numOfbombs; i++)
                    {
                        Vector3 dir = (bombDirection * cos(dTheta * i) + yDotDot * sin(dTheta * i)).Normalised();
                        Vector3 vel = dir * bombSpeed;
                        releaseBomb(vel, bombScale);
                    }
                    return true;
                }
                return false;
            }

            bool UseLaserOnPlayer(PlayerObject* player)
            {
                Vector3 bombScale{ 2,2,2 };
                float bombSpeed = 40.0f;
                Vector3 bombDirection = (player->GetTransform().GetGlobalPosition() - this->GetTransform().GetGlobalPosition()).Normalised();
                Vector3 bombVelocity = bombDirection * bombSpeed;
                float period = 2.0f;
                laserTimer += deltaTime;
                if (laserTimer > period)
                {
                    laserTimer = 0.0f;
                    float dRow = 10.0f;
                    float dColumn = 10.0f;
                    Vector3 upDir{ 0,1,0 };
                    Vector3 leftDir = Maths::Vector3::Cross(upDir, bombDirection);
                    for (int i = 0; i < 5; i++)
                    {
                        for (int j = 0; j < 5; j++)
                        {
                            Vector3 v = bombVelocity + leftDir * dColumn * j + upDir * dRow * i;
                            releaseBomb(v, bombScale);
                        }
                    }
                    for (int i = 0; i < 5; i++)
                    {
                        for (int j = 1; j < 5; j++)
                        {
                            Vector3 v = bombVelocity - leftDir * dColumn * j + upDir * dRow * i;
                            releaseBomb(v, bombScale);
                        }
                    }
                    return true;
                }
                return false;
            }

            bool JumpTo(PlayerObject* player)
            {
                float hangTime = 5.0f;
                if (jumpToTimer == 0.0f)
                {
                    float jumpSpeed = 35.0f;
                    const float PI = 3.1415926f;
                    float dTheta = PI / 4;
                    Vector3 playerDir = (player->GetTransform().GetGlobalPosition() - this->GetTransform().GetGlobalPosition()).Normalised();
                    Vector3 y{ 0,1,0 };
                    Vector3 yDot = Maths::Vector3::Cross(Maths::Vector3::Cross(playerDir, y), playerDir).Normalised();
                    Vector3 dir = (playerDir * cos(dTheta) + yDot * sin(dTheta)).Normalised();
                    Vector3 vel = dir * jumpSpeed;
                    this->GetPhysicsObject()->SetLinearVelocity(vel);
                }
                jumpToTimer += deltaTime;
                if (jumpToTimer > hangTime)
                {
                    jumpToTimer = 0.0f;
                    return true;
                }
                return false;
            }

            bool JumpAway(PlayerObject* player)
            {
                float hangTime = 5.0f;
                if (jumpAwayTimer == 0.0f)
                {
                    float jumpSpeed = 35.0f;
                    const float PI = 3.1415926f;
                    float dTheta = PI / 4;
                    Vector3 playerDir = (player->GetTransform().GetGlobalPosition() - this->GetTransform().GetGlobalPosition()).Normalised();
                    Vector3 y{ 0,1,0 };
                    Vector3 yDot = Maths::Vector3::Cross(Maths::Vector3::Cross(-playerDir, y), -playerDir).Normalised();
                    Vector3 dir = (-playerDir * cos(dTheta) + yDot * sin(dTheta)).Normalised();
                    Vector3 vel = dir * jumpSpeed;
                    this->GetPhysicsObject()->SetLinearVelocity(vel);
                }
                jumpAwayTimer += deltaTime;
                if (jumpAwayTimer > hangTime)
                {
                    jumpAwayTimer = 0.0f;
                    return true;
                }
                return false;
            }

            bool SeekHeal(bool& hasHeal)
            {
                float speed = 35.0f;
                std::vector<HealingKit*> healingKits = gameGrid->GetHealingKits();
                if (healingKits.size() == 0)
                {
                    hasHeal = false;
                    return false;
                }
                hasHeal = true;

                HealingKit* closest = healingKits[0];
                for (const auto& k : healingKits)
                {
                    if ((k->GetTransform().GetGlobalPosition() - this->GetTransform().GetGlobalPosition()).Length() < (closest->GetTransform().GetGlobalPosition() - this->GetTransform().GetGlobalPosition()).Length())
                    {
                        closest = k;
                    }
                }
                Chase(speed, closest->GetTransform().GetGlobalPosition(), gameGrid, deltaTime);
                return true;
            }

            float GetHealth()
            {
                return health;
            }

            void SetHealth(float hpNew)
            {
                health = hpNew;
            }

            void AddHealth(float hpChanges)
            {
                health += hpChanges;
            }

            bool InkSea()
            {
                float inkSeaDuration = 5.0f;
                inkSeaTimer += deltaTime;
                if (inkSeaTimer > inkSeaDuration)
                {
                    inkSeaTimer = 0.0f;
                    usingInkSea = false;
                    return true;
                }
                usingInkSea = true;
                return false;
            }

            bool BulletsStorm()
            {
                float bulletsStormDuration = 5.0f;
                float bulletsStormPeriod = 0.1f;
                Vector3 bombScale{ 1,1,1 };
                float bombSpeed = 30.0f;

                if (bulletsStormTimer < bulletsStormDuration)
                {
                    bulletsStormTimer += deltaTime;
                    bulletsStormFrequencyTimer += deltaTime;
                    if (bulletsStormFrequencyTimer > bulletsStormPeriod)
                    {
                        bulletsStormFrequencyTimer = 0.0f;
                        const float PI = 3.1415926;
                        int rayNum = 16;
                        bulletsStormAngle += PI / 50;
                        float currentPhase = bulletsStormAngle;
                        float dAngle = (2 * PI) / rayNum;
                        for (; currentPhase < (2 * PI + bulletsStormAngle); currentPhase += dAngle)
                        {
                            Vector3 rayDir = this->GetTransform().GetGlobalOrientation() * Vector3(cos(currentPhase), 0, sin(currentPhase));
                            Vector3 bombVelocity = rayDir * bombSpeed;
                            releaseBomb(bombVelocity, bombScale);
                        }
                        this->GetPhysicsObject()->SetAngularVelocity(this->GetTransform().GetGlobalOrientation() * Vector3 { 1, 1, 1 });
                    }
                    return false;
                }
                bulletsStormTimer = 0.0f;
                return true;
            }

            std::vector<Bomb*> GetBombsReleasedByBoss()
            {
                return bombsReleased;
            }

            void clearBombList()
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

            std::vector<Bomb*> bombsReleased;

            // Boss' attributes:
            float health = 100.0f;

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
                        //std::cout << "The boss has dead, and it should do nothing.\n";
                        // Note that, for current implementation, once the boss has dead, its action remain in Dead forever.
                        break;
                    case RandomWalk:
                        //std::cout << "Boss is walking randomly.\n";
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
                    if ((lowerLimit < boss->GetHealth()) && (boss->GetHealth() <= upperLimit))
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
