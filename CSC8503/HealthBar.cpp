#include"HealthBar.h"
using namespace NCL;
using namespace NCL::CSC8503;
HealthBar::HealthBar(std::shared_ptr<TextureBase> texture, Vector2 screenPos, Vector2 dimension, Health* health): UIObject(texture), health(health) {
		this->screenPos = screenPos;
		this->dimension = dimension;
}
