#include"HealthBar.h"
using namespace NCL;
using namespace NCL::CSC8503;
HealthBar::HealthBar(std::shared_ptr<TextureBase> texture, Vector2 screenPos, Vector2 dimension) : UIObject(texture) {
		this->screenPos = screenPos;
		this->dimension = dimension;
}
//void HealthBar::init() {
//
//}
