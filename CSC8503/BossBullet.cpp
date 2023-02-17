#include "BossBullet.h"
#include "Boss.h"
#include "InkEffectManager.h"
#include "GameGridManager.h"

using namespace NCL;
using namespace CSC8503;

BossBullet::BossBullet(Vector3 vel) : Bullet() {
    velocity = vel;
    lifeSpan = 5.0f;
    colour = Vector3(1, 0, 0);
}

BossBullet::~BossBullet() {
}

void BossBullet::Update(float dt) {
    this->GetTransform().SetPosition(this->GetTransform().GetGlobalPosition() + velocity * dt);
    lifeSpan -= dt;
    if (lifeSpan < 0.0f)
    {
        Delete();
    }
    if (this->GetTransform().GetGlobalPosition().y < 1.0f)
    {
        GetTransform().SetPosition({ GetTransform().GetGlobalPosition().x, 1.0f, GetTransform().GetGlobalPosition().z });
    }
}

void BossBullet::OnTriggerBegin(GameObject* other) {
    Bullet::OnTriggerBegin(other);
    if (PlayerObject* player = dynamic_cast<PlayerObject*>(other)) {
        lifeSpan = -1.0f;
        player->GetHealth()->Damage(bulletDamage);
    }
    if (Obstacle* obstacle = dynamic_cast<Obstacle*>(other)) {
        lifeSpan = -1.0f;
        obstacle->Damage(bulletDamage);
    }

    if (!dynamic_cast<Boss*>(other)) {
        GameGridManager::instance().PaintPosition(this->GetTransform().GetGlobalPosition(), paintHell::InkType::BossDamage);
    }
}
