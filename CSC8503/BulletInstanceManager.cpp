#include "BulletInstanceManager.h"
#include "BossBullet.h"
#include "PlayerBullet.h"
#include "PrefabLibrary.h"
#include "GameWorld.h"
#include "CollisionVolume.h"
#include "NetworkObject.h"


using namespace NCL::CSC8503;
BulletInstanceManager::BulletInstanceManager() {
}

BulletInstanceManager::~BulletInstanceManager() {
	//object deletion is handled by game world
}


Bullet* BulletInstanceManager::GetBullet(int firstIndex) {
	int index = indexs[firstIndex];
	//std::cout << index << std::endl;
	indexs[firstIndex] = (index + 1)% BULLETCOUNT;
	return bullets[firstIndex][index];
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