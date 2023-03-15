 #pragma once

#include"MenuRenderObject.h"
#include"UIObject.h"
#include "Health.h"

namespace NCL {
	namespace CSC8503 {
		class HealthBar : public UIObject
		{
		public:
			HealthBar(std::shared_ptr<TextureBase> texture, Vector2 screenPos, Vector2 dimension, Health* health = nullptr);
			~HealthBar() {

			};
			void Update(float dt) override {
			}
			Vector4 GetDimension() const override { 
				if (!health) {
					return Vector4(screenPos, dimension);
				}
				return Vector4(Vector2(screenPos.x + dimension.x * (health->GetHealth() / health->GetMaxHealth() - 1 ), screenPos.y), Vector2(dimension.x * health->GetHealth() / health->GetMaxHealth(), dimension.y));
			};
			bool IsEnabled = true;
		private:
			Health* health;
			
			Vector2 screenPos;
			Vector2 dimension;



		};
	}
}
