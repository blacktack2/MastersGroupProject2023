
/*********************************************************************
 * Author: Xiaoyang Liu
 * Date:   February 2023
 *********************************************************************/

#pragma once
#include "Boss.h"

#include "AssetLibrary.h"

void Boss::Update(float dt)
{
    health.Update(dt);
    deltaTime = dt;
    // check if inked
    //if (onGround) {
    GameNode* node = GameGridManager::instance().NearestNode(this->GetTransform().GetGlobalPosition());
    InkEffectManager::instance().ApplyInkEffect(node->inkType, GetHealth(), 1);
    //}
    //check boss health
    if (GetHealth()->GetHealth() <= 0) {
        gameStateManager->SetGameState(GameState::Win);
    }
}

void Boss::Chase(float speed, Vector3 destination, GameGrid* gameGrid, float dt)
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

BossBullet* Boss::releaseBossBullet(Vector3 v, Vector3 s, Vector3 p)
{
    BossBullet* ink = new BossBullet(*(BossBullet*)AssetLibrary::GetPrefab("bossBullet"));
    ink->SetLifespan(5.0f);
    Vector3 position = this->GetTransform().GetGlobalPosition();
    if (p != Vector3{ 99999,99999,99999 })
    {
        position = p;
    }
    ink->GetTransform()
        .SetPosition(position);
    ink->Resize(s);
    ink->GetPhysicsObject()->SetInverseMass(1.0f);
    ink->GetPhysicsObject()->SetGravWeight(0);
    ink->GetPhysicsObject()->SetDampingWeight(0);
    //ink->GetPhysicsObject()->SetLinearVelocity(this->physicsObject->GetLinearVelocity() * Vector3(1, 0, 1));
    ink->GetPhysicsObject()->ApplyLinearImpulse(v);
    if (s.Length() >= 1) {
        //ink->SetDamage(10);
    }
    GameWorld& gameWorld = GameWorld::instance();
    gameWorld.AddGameObject(ink);

    return (BossBullet*)ink;

    /*
    BossBullet* bomb = new BossBullet(v);
    SphereVolume* volume = new SphereVolume(s.x);
    bomb->SetBoundingVolume((CollisionVolume*)volume);
    Vector3 position = this->GetTransform().GetGlobalPosition();
    bomb->GetTransform()
        .SetPosition(position)
        .SetScale(s);
    bomb->SetPhysicsObject(new PhysicsObject(&bomb->GetTransform(), bomb->GetBoundingVolume(), true));
    bomb->GetPhysicsObject()->SetInverseMass(0.0f);
    bomb->GetPhysicsObject()->InitSphereInertia();
        
                
    bombsReleased.push_back(bomb);

    return bomb;
    */
}

bool Boss::RandomWalk()   // TODO: the boss should not go outside the boundaries
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

bool Boss::StabPlayer(PlayerObject* player)
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
        releaseBossBullet(bombVelocity, bombScale);
        Vector3 y{ 0,1,0 };
        Vector3 yDot = Maths::Vector3::Cross(Maths::Vector3::Cross(bombDirection, y), bombDirection).Normalised();
        for (int i = 1; i < numOfbombs; i++)
        {
            Vector3 dir = (bombDirection * cos(dTheta * i) + yDot * sin(dTheta * i)).Normalised();
            Vector3 vel = dir * bombSpeed;
            releaseBossBullet(vel, bombScale);
        }
        return true;
    }
    return false;
}

bool Boss::Spin(PlayerObject* player)
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
        releaseBossBullet(bombVelocity, bombScale);
        Vector3 y{ 0,1,0 };
        Vector3 leftDir = Maths::Vector3::Cross(y, bombDirection);
        Vector3 yDot = Maths::Vector3::Cross(Maths::Vector3::Cross(bombDirection, leftDir), bombDirection).Normalised();
        Vector3 yDotDot = Maths::Vector3::Cross(Maths::Vector3::Cross(bombDirection, -leftDir), bombDirection).Normalised();
        for (int i = 1; i < numOfbombs; i++)
        {
            Vector3 dir = (bombDirection * cos(dTheta * i) + yDot * sin(dTheta * i)).Normalised();
            Vector3 vel = dir * bombSpeed;
            releaseBossBullet(vel, bombScale);
        }
        for (int i = 1; i < numOfbombs; i++)
        {
            Vector3 dir = (bombDirection * cos(dTheta * i) + yDotDot * sin(dTheta * i)).Normalised();
            Vector3 vel = dir * bombSpeed;
            releaseBossBullet(vel, bombScale);
        }
        return true;
    }
    return false;
}

bool Boss::UseLaserOnPlayer(PlayerObject* player)
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
                releaseBossBullet(v, bombScale);
            }
        }
        for (int i = 0; i < 5; i++)
        {
            for (int j = 1; j < 5; j++)
            {
                Vector3 v = bombVelocity - leftDir * dColumn * j + upDir * dRow * i;
                releaseBossBullet(v, bombScale);
            }
        }
        return true;
    }
    return false;
}

bool Boss::JumpTo(PlayerObject* player)
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

bool Boss::JumpAway(PlayerObject* player)
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

bool Boss::SeekHeal(bool& hasHeal)
{
    // TODO

                /*float speed = 35.0f;
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
                return true;*/

    hasHeal = false;
    return false;
}

bool Boss::InkRain(PlayerObject* player)
{
    float rainPeriod = 0.1f;
    int rainRange = 30;
    int numOfBomb = 30;
    float bombSpeed = 100.0f;
    Vector3 bombScale{ 0.5,0.5,0.5 };
    Vector3 startingPosition = this->GetTransform().GetGlobalPosition();
    if (!rainIsInitialised)
    {
        rain = std::vector<BossBullet*>(numOfBomb);
        for (int n = 0; n < numOfBomb; n++)
        {
            float xDot = std::rand() % rainRange + 1;
            float xDotDot = -(std::rand() % rainRange + 1);
            float yDot = std::rand() % rainRange + 1;
            float zDot = std::rand() % rainRange + 1;
            float zDotDot = -(std::rand() % rainRange + 1);
            float dx = xDot + xDotDot;
            float dy = yDot;
            float dz = zDot + zDotDot;
            Vector3 bombPostion{ startingPosition.x + dx, startingPosition.y + dy, startingPosition.z + dz };
            rainBombPositions.push_back(bombPostion);   // we NEED this extra container to store the positions, otherwise it will cause memory violation!
            BossBullet* b = releaseBossBullet({ 0,0,0 }, bombScale, bombPostion);
            b->SetLifespan(99999.0f);
            rain[n] = b;
        }
        rainIsInitialised = true;
        return false;
    }
    if (currentRainBomb == rain.size())
    {
        rainBombPositions.clear();
        rainIsInitialised = false;
        inkRainTimer = 0.0f;
        currentRainBomb = 0;
        return true;
    }
    inkRainTimer += deltaTime;
    if (inkRainTimer > rainPeriod)
    {
        inkRainTimer = 0.0f;
        rain[currentRainBomb]->SetLifespan(0.0f);
        Vector3 bombDirection = (player->GetTransform().GetGlobalPosition() - rainBombPositions[currentRainBomb]).Normalised();
        Vector3 bombVelocity = bombDirection * bombSpeed;
        BossBullet* b = releaseBossBullet(bombVelocity, bombScale, rainBombPositions[currentRainBomb]);
        b->SetLifespan(5.0f);
        rain[currentRainBomb] = b;
        currentRainBomb++;
    }
    return false;
}

bool Boss::BulletsStorm()
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
                releaseBossBullet(bombVelocity, bombScale);
            }
            this->GetPhysicsObject()->SetAngularVelocity(this->GetTransform().GetGlobalOrientation() * Vector3 { 1, 1, 1 });
        }
        return false;
    }
    bulletsStormTimer = 0.0f;
    return true;
}