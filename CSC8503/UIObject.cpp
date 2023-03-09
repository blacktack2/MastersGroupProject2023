/**
 * @file   UIObject.cpp
 * @brief  See UIObject.h.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#include "UIObject.h"

using namespace NCL;
using namespace NCL::CSC8503;
using namespace NCL::Rendering;

UIObject::UIObject(std::shared_ptr<TextureBase> texture) {
	renderObject = std::make_unique<MenuRenderObject>(texture);
}

void UIObject::Draw() {
	renderObject->Draw(GetDimension());
	DrawExtras();
	for (UIObject& child : children) {
		child.Draw();
	}
}

MenuRenderObject& UIObject::GetRenderObject() const {
	return *renderObject;
}

bool UIObject::HasRenderObject() const {
	return renderObject != nullptr;
}
