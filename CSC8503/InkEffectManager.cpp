#include "InkEffectManager.h"
#include "Vector4.h"



using namespace NCL;
using namespace CSC8503;
using namespace Maths;

InkEffectManager::InkEffectManager(){
}

InkEffectManager::~InkEffectManager() {
}

void InkEffectManager::Update(float dt) {

}

Vector3 InkEffectManager::GetColour(NCL::InkType type) {
	switch (type)
	{
	case NCL::InkType::NoInk:
	default:
		return Vector3(1, 1, 1);
		break;
	case NCL::InkType::BossDamage:
		return Vector3(255 / 255.0f, 63 / 255.0f, 10 / 255.0f);
		break;
	case NCL::InkType::PlayerDamage:
		return Vector3(3 / 255.0f, 66 / 255.0f, 195 / 255.0f);
		break;
	}
}

void InkEffectManager::ApplyInkEffect(NCL::InkType type, Health* health, int healthType) {
	switch (type)
	{
		case NCL::InkType::NoInk:
		default:
			break;
		case NCL::InkType::BossDamage:
			if (healthType == 0) {
				health->DamageOverTime(damage, 0.1f);
			}
			break;
		case NCL::InkType::PlayerDamage:
			if (healthType == 1) {
				health->DamageOverTime(damage, 0.1f);
			}
			break;
	}
}
