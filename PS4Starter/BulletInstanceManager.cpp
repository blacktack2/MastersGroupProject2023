#include "BulletInstanceManager.h"
#ifdef x64
#include "BossBullet.h"
#include "PlayerBullet.h"
#include "PrefabLibrary.h"
#include "GameWorld.h"
#include "CollisionVolume.h"
#include "NetworkObject.h"
#endif // x64

#ifdef _ORBIS

#include "Bullet.h"
#include "BossBullet.h"
#include "PlayerBullet.h"
#include "../Common/PhysicsObject.h"
#include "../Common/GameWorld.h"
#include "../Common/PrefabLibrary.h"
#include "../Common/CollisionVolume.h"

#endif // _ORBIS




using namespace NCL::CSC8503;
BulletInstanceManager::BulletInstanceManager() {
}

BulletInstanceManager::~BulletInstanceManager() {
	//object deletion is handled by game world
}


Bullet* BulletInstanceManager::GetBullet(int firstIndex) {
	int index = indexs[firstIndex];
	//std::cout << index << std::endl;
	indexs[firstIndex] = (index + 1) % BULLETCOUNT;
	Bullet* bullet = bullets[firstIndex][index];
	bullet->GetPhysicsObject()->ClearForces();
	bullet->GetPhysicsObject()->SetLinearVelocity(Vector3(0.0f));
	bullet->GetPhysicsObject()->SetAngularVelocity(Vector3(0.0f));
	return bullet;
}

void BulletInstanceManager::ObjectIntiation() {
	Bullet* newBullet;
	GameWorld& world = GameWorld::instance();
	for (size_t i = 0; i < BULLETCOUNT; i++)
	{
		newBullet = new PlayerBullet(*(PlayerBullet*)&PrefabLibrary::GetPrefab("bullet"));
		newBullet->SetActive(false);
		world.AddGameObject(newBullet);
		bullets[0][i] = newBullet;
		newBullet = new BossBullet(*(BossBullet*)&PrefabLibrary::GetPrefab("bossBullet"));
		newBullet->SetActive(false);
		world.AddGameObject(newBullet);
		bullets[1][i] = newBullet;
	}
	for (size_t i = 0; i < BULLETTYPECOUNT; i++)
	{
		indexs[i] = 0;
	}
}
#ifdef x64
void BulletInstanceManager::AddNetworkObject(int startInt) {
	GameWorld& world = GameWorld::instance();
	for (size_t i = 0; i < BULLETTYPECOUNT; i++)
	{
		for (auto bullet : bullets[i])
		{
			bullet->SetNetworkObject(new NetworkObject(static_cast<GameObject&>(*bullet), startInt));
			world.AddNetworkObject(bullet->GetNetworkObject());
			startInt++;
		}
	}
}
#endif // x64

