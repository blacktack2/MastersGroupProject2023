 #pragma once

#include"MenuRenderObject.h"
#include"AssetLibrary.h"
#include"UIObject.h"

namespace NCL {
	namespace CSC8503 {
		class HealthBar:public UIObject
		{
		public:
			HealthBar(std::shared_ptr<TextureBase> texture, Vector2 screenPos, Vector2 dimension);
			~HealthBar() {

			};
			//HealthBar(Vector2 screenPos, Vector2 dimension) {
			//	this->screenPos = screenPos;
			//	this->dimension = dimension;
			//}
			void Update(float dt) override {

			}
			Vector4 GetDimension() const override { return Vector4(screenPos, dimension); };

		private:
			//std::map<int, HealthBar*> huds;
			
			Vector2 screenPos;
			Vector2 dimension;

		};
	}
}
