/**
 * @file   Boss.cpp
 * @brief  See Boss.h.
 *
 * @author Xiaoyang Liu
 * @author Felix Chiu
 * @date   February 2023
 */
#pragma once
#include "Boss.h"
#include "BulletInstanceManager.h"
#include "AssetLibrary.h"
#include "PrefabLibrary.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourNodeWithChildren.h"
#include "BehaviourAction.h"
#include "DebugViewPoint.h"
#include <limits>
#include "SoundSource.h"
#include "Sound.h"
#include "SoundSystem.h"

const float offensiveHealthLowerBound = 50;
const float bossVision = 90;
const float distanceToHaveCloseCombat = 40;

Boss::Boss() {
    BuildTree();
    SetupAudio();
}

Boss::~Boss() {
    delete behaviourTree;

    delete hurtSource;
    delete inkRainSource;
    delete fatherSource;
    delete wowSource;
    delete tuturuSource;
    delete senpaiSource;
    delete nyaSource;
    delete waitSource;
}

void Boss::BuildTree() {
    

    behaviourTree = new BehaviourSelector("Root");

    BehaviourSequence* offensiveMode = new BehaviourSequence("Offensive sub-tree");
    behaviourTree->AddChild(offensiveMode);

    offensiveMode->AddChild(new BehaviourAction("Check Boss Health for offensive mode", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        }

        if (health.GetHealth() < offensiveHealthLowerBound) return Failure;
        return Success;
        }));

    BehaviourSelector* offensinveMoves = new BehaviourSelector("Offensive Moves");
    offensiveMode->AddChild(offensinveMoves);

    offensinveMoves->AddChild(new BehaviourAction("Walk if player to far away", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        }

        if (SqrDistToTarget() > bossVision * bossVision || state == Ongoing)
        {
            if (RandomWalk()) return Success;
            else { 
                bossAction = Move1;
                return Ongoing; 
            }
        }
        return Failure;
        }));

    BehaviourSequence* offensiveClose = new BehaviourSequence("Offensive Close Combat");
    offensinveMoves->AddChild(offensiveClose);


    //offensive close actions
    offensiveClose->AddChild(new BehaviourAction("Check Player Dist for close Combat", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        }

        if (SqrDistToTarget() < distanceToHaveCloseCombat * distanceToHaveCloseCombat) return Success;
        return Failure;
        }));

    BehaviourSelector* offensiveCloseAction = new BehaviourSelector("Choose Offensive Close Combat");
    offensiveClose->AddChild(offensiveCloseAction);

    offensiveCloseAction->AddChild(new BehaviourAction("Stab Player", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        case Initialise:
            if (std::rand() % 100 > 60) return Failure;
        }
        if (StabPlayer(target)) return Success;
        bossAction = Attack1;
        return Ongoing;
        }));

    offensiveClose->AddChild(new BehaviourAction("Spin On Player", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        }

        if (Spin(target)) return Success;
        bossAction = Attack2;
        return Ongoing;
    }));


    //offensive remote actions
    offensinveMoves->AddChild(new BehaviourAction("Laser Player", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        case Initialise:
            //if too much laser auto fail
            //if too many jump  auto success
            if ((std::rand() % 100 > 50 && consecutiveJumpCounter <= 3) || consecutiveLaserCounter >= 3) {
                return Failure;
            }
            //if (std::rand() % 100 > 70) return Failure;
        }
        if (UseLaserOnPlayer(target)) {
            consecutiveLaserCounter++;
            consecutiveJumpCounter = 0;
            return Success;
        }
        bossAction = Attack3;
        return Ongoing;
    }));

    offensinveMoves->AddChild(new BehaviourAction("Jump To Player", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        }

        if (JumpTo(target)) {
            consecutiveJumpCounter++;
            consecutiveLaserCounter = 0;
            return Success;
        }
        bossAction = Move2;
        return Ongoing;
    }));

    //Deffensive Sub tree
    behaviourTree->AddChild(new BehaviourAction("Walk if player to far awway", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        }

        if (SqrDistToTarget() > bossVision * bossVision || state == Ongoing)
        {
            if (RandomWalk()) return Success;
            else {
                bossAction = Move1;
                return Ongoing;
            }
        }
        return Failure;
    }));

    behaviourTree->AddChild(new BehaviourAction("Jump away if player is close", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        }

        if (SqrDistToTarget() < distanceToHaveCloseCombat * distanceToHaveCloseCombat || state == Ongoing)
        {
            if (JumpAway(target)) return Success;
            else {
                bossAction = Move3;
                return Ongoing;
            }
        }
        return Failure;
    }));
    //Defensive Attacks
    behaviourTree->AddChild(new BehaviourAction("Ink Rain", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        case Initialise:
            if (std::rand() % 100 > 50) return Failure;
        }
        if (InkRain(target)) return Success;
        bossAction = Attack4;
        return Ongoing;
    }));

    behaviourTree->AddChild(new BehaviourAction("Bullet Storm", [&](float dt, BehaviourState state)->BehaviourState {
        switch (state)
        {
        case Failure:
        case Success:
            return state;
        }

        if (BulletsStorm()) return Success;
        bossAction = Attack5;
        return Ongoing;
    }));
}

void Boss::SetupAudio()
{
    hurtSource = new SoundSource();
    hurtSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
    hurtSource->SetGain(0.0f);
    hurtSource->SetSoundBuffer(Sound::AddSound("tom.wav"));
    hurtSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
    //hurtSource->SetTarget(this);
    //hurtSource->SetMaxDistance(10.0f);
    hurtSource->SetGain(1.0f);
    hurtSource->SetPitch(1.0f);
    //SoundSystem::GetSoundSystem()->AddSoundSource(hurtSource);

    inkRainSource = new SoundSource();
    inkRainSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
    inkRainSource->SetGain(0.0f);
    //inkRainSource->SetMaxDistance(10.0f);
    inkRainSource->SetSoundBuffer(Sound::AddSound("inkRain.wav"));
    inkRainSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
    inkRainSource->SetGain(1.0f);
    inkRainSource->SetPitch(1.0f);

    fatherSource = new SoundSource();
    fatherSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
    fatherSource->SetGain(0.0f);
    fatherSource->SetSoundBuffer(Sound::AddSound("father.wav"));
    fatherSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
    fatherSource->SetGain(1.0f);
    fatherSource->SetPitch(1.0f);

    wowSource = new SoundSource();
    wowSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
    wowSource->SetGain(0.0f);
    wowSource->SetSoundBuffer(Sound::AddSound("wow.wav"));
    wowSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
    wowSource->SetGain(1.0f);
    wowSource->SetPitch(1.0f);

    tuturuSource = new SoundSource();
    tuturuSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
    tuturuSource->SetGain(0.0f);
    tuturuSource->SetSoundBuffer(Sound::AddSound("tuturu.wav"));
    tuturuSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
    tuturuSource->SetGain(1.0f);
    tuturuSource->SetPitch(1.0f);

    senpaiSource = new SoundSource();
    senpaiSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
    senpaiSource->SetGain(0.0f);
    senpaiSource->SetSoundBuffer(Sound::AddSound("senpai.wav"));
    senpaiSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
    senpaiSource->SetGain(1.0f);
    senpaiSource->SetPitch(1.0f);

    nyaSource = new SoundSource();
    nyaSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
    nyaSource->SetGain(0.0f);
    nyaSource->SetSoundBuffer(Sound::AddSound("nya.wav"));
    nyaSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
    nyaSource->SetGain(1.0f);
    nyaSource->SetPitch(1.0f);

    waitSource = new SoundSource();
    waitSource->SetPriority(SoundPriority::SOUNDPRIORITY_ALWAYS);
    waitSource->SetGain(0.0f);
    waitSource->SetSoundBuffer(Sound::AddSound("wait.wav"));
    waitSource->AttachSource(SoundSystem::GetSoundSystem()->GetSource());
    waitSource->SetGain(1.0f);
    waitSource->SetPitch(1.0f);

}

void Boss::Update(float dt) {
    NCL::DebugViewPoint::Instance().MarkTime("Boss Update");
    GetTarget();
    BossAction prevState = bossAction;
    health.Update(dt);
    deltaTime = dt;
    // check if inked
    GameNode* node = GameGridManager::instance().NearestNode(this->GetTransform().GetGlobalPosition());
    if (node) {
        if (this->GetTransform().GetGlobalPosition().y <= 10) {
            InkEffectManager::instance().ApplyInkEffect(node->inkType, GetHealth(), 1);
            if (node->inkType == NCL::InkType::PlayerDamage) {
                hurtSource->Play(Sound::AddSound("tom.wav"));
            }
        }
    }
    //check boss health
    if (gameStateManager->GetGameState() == GameState::OnGoing) {
        if (GetHealth()->GetHealth() <= 0) {
            ChangeLoseState();
        }
        if (this->transform.GetGlobalPosition().y < -10) {
            health.SetHealth(0);
            ChangeLoseState();
        }
    }

    if (health.GetHealth() > 0 && isSpawnBullet)
    {
        bossAction = NoAction;
        if (behaviourTree->Execute(dt) != Ongoing) {
            behaviourTree->Reset();
        }
    }

    PlayAnimation();

    //std::cout << "Boss doing " << bossAction << std::endl;

    if (bossAction != prevState) {
        ChangeTarget();
    }
    NCL::DebugViewPoint::Instance().FinishTime("Boss Update");
}

void Boss::ChangeLoseState()
{
    if (isSpawnBullet) {
        bossAction = Dead;
        gameStateManager->SetGameState(GameState::Win);
    }
   
}

void Boss::Chase(float speed, Vector3 destination, GameGrid* gameGrid, float dt) {
    if (gameGrid == nullptr) {
        return;
    }

    float aiSpeed = speed;

    NavigationPath samples;
    Vector3 node;
    std::vector<Vector3> tempSteps;
    while (samples.PopWaypoint(node)) {
        tempSteps.push_back(node);
    }
    if (tempSteps.size() <= 100) {  // enemy is too close and will just go straight to the destination
        this->GetTransform().SetPosition(this->GetTransform().GetGlobalPosition() + ((destination - this->GetTransform().GetGlobalPosition()).Normalised() * (dt * aiSpeed)));
    } else {
        Vector3 closest{ 9999, 9999, 9999 };
        Vector3 forward;
        for (size_t n = 0; n < tempSteps.size() - 1; n++) {
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
        for (size_t i = 1; i < tempSteps.size(); i++) {
            Vector3 a = tempSteps[i - 1];
            Vector3 b = tempSteps[i];

            //Debug::DrawLine(a, b, Vector4{ 0,1,0,1 }, 0.0f);
        }
    }
}

BossBullet* Boss::releaseBossBullet(Vector3 v, Vector3 s, Vector3 p) {
    if (!isSpawnBullet) {
        return nullptr;
    }
    BossBullet* ink = BulletInstanceManager::instance().GetBossBullet();
    ink->GetPhysicsObject()->SetAngularVelocity(Vector3{20,20,20});
    ink->SetLifespan(5.0f);
    Vector3 position = this->GetTransform().GetGlobalPosition();
    if (p != Vector3{ 99999, 99999, 99999 }) {
        position = p;
    }
    ink->GetTransform()
        .SetPosition(position);
    ink->Resize(s);
    ink->SetPaintRadius(s.x * 1.8);
    ink->GetPhysicsObject()->SetInverseMass(1.0f);
    ink->GetPhysicsObject()->SetGravWeight(0);
    ink->GetPhysicsObject()->SetDampingWeight(0);
    //ink->GetPhysicsObject()->SetLinearVelocity(this->physicsObject->GetLinearVelocity() * Vector3(1, 0, 1));
    ink->GetPhysicsObject()->ApplyLinearImpulse(v);
    if (s.Length() >= 1) {
        //ink->SetDamage(10);
    }
    ink->SetActive(true);
    BulletModification(ink);
    return ink;

}

void Boss::ChangeTarget(){
    if (nextTarget != target) {
        target = nextTarget;
    }
}

void Boss::SetBossOrientation(Vector3* facingDir) {
    Vector3 dir;
    if (!facingDir) {
        dir = target->GetTransform().GetGlobalPosition() - this->GetTransform().GetGlobalPosition();
    }
    else {
        dir = *facingDir;
    }
    dir.y = 0;
    Quaternion orientation = Quaternion(Matrix4::BuildViewMatrix(this->GetTransform().GetGlobalPosition() + dir * 10, this->GetTransform().GetGlobalPosition(), Vector3(0, 1, 0)).Inverse());
    if (isSpawnBullet) {
        this->GetTransform().SetOrientation(orientation);
    }
}

bool Boss::RandomWalk() {
    float speed = 20.0f;
    float period = 0.5f;    // change direction in period-seconds
    randomWalkTimer += deltaTime;
    if (randomWalkTimer > period) {
        randomWalkTimer = 0.0f;
        float x = (float)((std::rand() % 11) - 5);
        float z = (float)((std::rand() % 11) - 5);
        randomWalkDirection = Vector3{ x,0,z };
        randomWalkDirection = randomWalkDirection.Normalised();

        SetBossOrientation(&randomWalkDirection);
        //Quaternion orientation = Quaternion(Matrix4::BuildViewMatrix(this->GetTransform().GetGlobalPosition() + randomWalkDirection * 10, this->GetTransform().GetGlobalPosition(), Vector3(0, 1, 0)).Inverse());
        //this->GetTransform().SetOrientation(orientation);
        return true;
    }
    //Debug::DrawLine(GetTransform().GetGlobalPosition(), GetTransform().GetGlobalPosition() + randomWalkDirection * 15, Vector4{ 0,1,0,1 }, 0.0f);
    GetTransform().SetPosition(GetTransform().GetGlobalPosition() + randomWalkDirection * speed * deltaTime);
    return false;
}

bool Boss::StabPlayer(PlayerObject* player) {
    SetBossOrientation(nullptr);

    Vector3 bombScale{ 0.75,0.75,0.75 };
    float bombSpeed = 40.0f;
    Vector3 bombDirection = (player->GetTransform().GetGlobalPosition() - this->GetTransform().GetGlobalPosition()).Normalised();
    Vector3 bombVelocity = bombDirection * bombSpeed;
    float period = 1.0f;
    int numOfbombs = 10;
    stabTimer += deltaTime;
    if (stabTimer > period) {
        stabTimer = 0.0f;
        const float PI = 3.1415926f;
        float dTheta = PI / 64;
        releaseBossBullet(bombVelocity, bombScale);
        Vector3 y{ 0,1,0 };
        Vector3 yDot = Maths::Vector3::Cross(Maths::Vector3::Cross(bombDirection, y), bombDirection).Normalised();
        for (int i = 1; i < numOfbombs; i++) {
            Vector3 dir = (bombDirection * cos(dTheta * i) + yDot * sin(dTheta * i)).Normalised();
            Vector3 vel = dir * bombSpeed;
            releaseBossBullet(vel, bombScale);
        }
        return true;
    }
    return false;
}

bool Boss::Spin(PlayerObject* player) {
    SetBossOrientation(nullptr);
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

bool Boss::UseLaserOnPlayer(PlayerObject* player) {
    SetBossOrientation(nullptr);
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
                Vector3 v = bombVelocity + leftDir * dColumn * (float)j + upDir * dRow * (float)i;
                releaseBossBullet(v, bombScale);
            }
        }
        for (int i = 0; i < 5; i++)
        {
            for (int j = 1; j < 5; j++)
            {
                Vector3 v = bombVelocity - leftDir * dColumn * (float)j + upDir * dRow * (float)i;
                releaseBossBullet(v, bombScale);
            }
        }
        return true;
    }
    return false;
}

bool Boss::JumpTo(PlayerObject* player) {
    SetBossOrientation(nullptr);
    float hangTime = 5.0f;
    if (jumpToTimer == 0.0f) {
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
    if (jumpToTimer > hangTime) {
        jumpToTimer = 0.0f;
        return true;
    }
    return false;
}

bool Boss::JumpAway(PlayerObject* player) {
    SetBossOrientation(nullptr);
    float hangTime = 5.0f;
    if (jumpAwayTimer == 0.0f) {
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
    if (jumpAwayTimer > hangTime) {
        jumpAwayTimer = 0.0f;
        return true;
    }
    return false;
}

bool Boss::SeekHeal(bool& hasHeal) {
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

bool Boss::InkRain(PlayerObject* player) {
    SetBossOrientation(nullptr);
    float rainPeriod = 0.1f;
    int rainRange = 30;
    int numOfBomb = 30;
    float bombSpeed = 100.0f;
    Vector3 bombScale{ 0.5,0.5,0.5 };
    Vector3 startingPosition = this->GetTransform().GetGlobalPosition();
    if (!rainIsInitialised) {
        rain = std::vector<BossBullet*>(numOfBomb);
        for (int n = 0; n < numOfBomb; n++) {
            float xDot = (float)(std::rand() % rainRange + 1);
            float xDotDot = (float)-(std::rand() % rainRange + 1);
            float yDot = (float)(std::rand() % rainRange + 1);
            float zDot = (float)(std::rand() % rainRange + 1);
            float zDotDot = (float)-(std::rand() % rainRange + 1);
            float dx = xDot + xDotDot;
            float dy = yDot;
            float dz = zDot + zDotDot;
            Vector3 bombPostion{ startingPosition.x + dx, startingPosition.y + dy, startingPosition.z + dz };
            rainBombPositions.push_back(bombPostion);   // we NEED this extra container to store the positions, otherwise it will cause memory violation!
            BossBullet* b = releaseBossBullet({ 0,0,0 }, bombScale, bombPostion);
            if (b) {
                b->SetLifespan(99999.0f);
                rain[n] = b;
            }
            
        }
        rainIsInitialised = true;
        return false;
    }
    if (currentRainBomb == rain.size()) {
        rainBombPositions.clear();
        rainIsInitialised = false;
        inkRainTimer = 0.0f;
        currentRainBomb = 0;
        return true;
    }
    inkRainTimer += deltaTime;
    if (inkRainTimer > rainPeriod && isSpawnBullet) {
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

bool Boss::BulletsStorm() {
    SetBossOrientation(nullptr);
    float bulletsStormDuration = 5.0f;
    float bulletsStormPeriod = 0.1f;
    Vector3 bombScale{ 1,1,1 };
    float bombSpeed = 30.0f;

    if (bulletsStormTimer < bulletsStormDuration) {
        bulletsStormTimer += deltaTime;
        bulletsStormFrequencyTimer += deltaTime;
        if (bulletsStormFrequencyTimer > bulletsStormPeriod) {
            bulletsStormFrequencyTimer = 0.0f;
            const float PI = 3.1415926f;
            int rayNum = 16;
            bulletsStormAngle += PI / 50;
            float currentPhase = bulletsStormAngle;
            float dAngle = (2 * PI) / rayNum;
            for (; currentPhase < (2 * PI + bulletsStormAngle); currentPhase += dAngle) {
                Vector3 rayDir = transform.GetGlobalOrientation() * Vector3(cos(currentPhase), 0, sin(currentPhase));
                Vector3 bombVelocity = rayDir * bombSpeed;
                Vector3 p = this->GetTransform().GetGlobalPosition();
                p.y = p.y - 7;
                releaseBossBullet(bombVelocity, bombScale, p);
            }
            physicsObject->SetAngularVelocity(transform.GetGlobalOrientation() * Vector3 { 1, 1, 1 });
        }
        return false;
    }
    physicsObject->SetAngularVelocity(Vector3(0, 0, 0));
    Vector3 eurler = transform.GetGlobalOrientation().ToEuler();
    transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, eurler.y, 0));
    bulletsStormTimer = 0.0f;
    return true;
}


float Boss::SqrDistToTarget() {
    if (!target) return std::numeric_limits<float>::max();
    return (transform.GetGlobalPosition() - target->GetTransform().GetGlobalPosition()).LengthSquared();
}
float Boss::DistToTarget() {
    return  (transform.GetGlobalPosition() - target->GetTransform().GetGlobalPosition()).Length();
}

void Boss::PlayAnimation()
{
    AnimatedRenderObject* anim = static_cast<AnimatedRenderObject*>(GetRenderObject());
    switch (bossAction) {
    case Move1:
        if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("WalkForward").get()) {
            anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("WalkForward"));
        }

        break;
    case Attack1:
        if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("Attack1").get()) {
            anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("Attack1"));
        }
        nyaSource->Play(Sound::AddSound("nya.wav"));
        break;
    case Attack2:
        if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("Attack2").get()) {
            anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("Attack2"));
        }
        tuturuSource->Play(Sound::AddSound("tuturu.wav"));
        break;
    case Attack3:
        if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("Attack3").get()) {
            anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("Attack3"));
        }
        wowSource->Play(Sound::AddSound("wow.wav"));
        break;
    case Move2:
        if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("Jump").get()) {
            anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("Jump"));
        }
        senpaiSource->Play(Sound::AddSound("senpai.wav"));
        break;
    case Move3:
        if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("Jump").get()) {
            anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("Jump"));
        }
        waitSource->Play(Sound::AddSound("wait.wav"));
        break;
    case Attack4:
        if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("Attack6").get()) {
            anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("Attack6"));
        }
        inkRainSource->Play(Sound::AddSound("inkRain.wav"));
        break;
    case Attack5:
        if (&anim->GetAnimation() != AssetLibrary<MeshAnimation>::GetAsset("Attack5").get()) {
            anim->SetAnimation(AssetLibrary<MeshAnimation>::GetAsset("Attack5"));
        }
        fatherSource->Play(Sound::AddSound("father.wav"));
        break;
    }
}
