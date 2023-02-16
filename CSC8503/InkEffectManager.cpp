#include "InkEffectManager.h"

using namespace NCL;
using namespace CSC8503;

InkEffectManager::InkEffectManager(){
}

InkEffectManager::~InkEffectManager() {
}

void InkEffectManager::Update(float dt) {

}
void InkEffectManager::ApplyInkEffect(paintHell::InkType type, Health* health, int healthType) {
	switch (type)
	{
		case paintHell::InkType::NoInk:
		default:
			break;
		case paintHell::InkType::BossDamage:
			if (healthType == 0) {
				health->DamageOverTime(damage, 0.1f);
			}
			break;
		case paintHell::InkType::PlayerDamage:
			if (healthType == 1) {
				health->DamageOverTime(damage, 0.1f);
			}
			break;
	}
}
