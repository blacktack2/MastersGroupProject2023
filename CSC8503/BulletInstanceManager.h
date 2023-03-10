#pragma once
#include "BossBullet.h"
#include "PlayerBullet.h"

#define BULLETCOUNT 1000
#define BULLETTYPECOUNT 2
namespace NCL::CSC8503 {
	class Bullet;


	class BulletInstanceManager {
	public:
		static BulletInstanceManager& instance() {
			static BulletInstanceManager INSTANCE;
			return INSTANCE;
		}

		void ObjectIntiation();
		void AddNetworkObject(int startInt =0);

		void NullifyArray() {
			for (size_t i = 0; i < BULLETTYPECOUNT; i++)
			{
				for (size_t j = 0; j < BULLETCOUNT; j++)
				{
					bullets[i][j] = NULL;
				}
			}
		}

		PlayerBullet* GetPlayerBullet() {
			return (PlayerBullet*)GetBullet(0);
		}

		BossBullet* GetBossBullet() {
			return (BossBullet*)GetBullet(1);
		}


	private:
		BulletInstanceManager();
		~BulletInstanceManager();


		Bullet* GetBullet(int firstIndex);

		Bullet* bullets[2][BULLETCOUNT] = {nullptr};
		int indexs[2] = {0};
	};

}