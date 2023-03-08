/**
 * @file   UIObject.cpp
 * @brief  See UIObject.h.
 *
 * @author Felix Chiu
 * @date   February 2023
 */
#include "UIObject.h"

using namespace NCL;
using namespace CSC8503;

UIObject::UIObject() {
	renderObject = nullptr;
}

UIObject::~UIObject() {
	delete renderObject;
}


