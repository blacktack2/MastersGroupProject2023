#pragma once
#include "GameObject.h"
#include "InkEffectManager.h"

namespace NCL {
	namespace CSC8503 {
		class Bullet : public GameObject {
		public:
			typedef std::function<void(Bullet& bullet)> overlap_func;
			Bullet();
			Bullet(Bullet& other);
			~Bullet();

			virtual void Update(float dt) override;
			void OnTriggerBegin(GameObject* other) override;
			void OnCollisionBegin(GameObject* other) override;

			void OnDestroy();

			void UpdateColour();

			void SetLifespan(float ls) {
				lifespan = ls;
			}

			void Resize(Vector3 scale);

			void SetPaintRadius(float scale);
			float GetPaintRadius();

			overlap_func OnDestroyCallback = nullptr;

			void SetPaintEnable(bool isEnable) {
				paintEnable = isEnable;
			}

			Vector3 GetColour() {
				return colour;
			}
			NCL::InkType GetInkType() {
				return inkType;
			}
		protected:
			void PaintCollision(GameObject& other);

			float paintRadius = 2.0f;
			float lifespan = 0;
			Vector3 colour = Vector3(1, 0, 1);
			NCL::InkType inkType;

			bool paintEnable = true;
		};
	}
}
