#pragma once
#include <string>
#include "Transform.h"

namespace paintHell::core {

	class GameObject {
	public:
		GameObject();
		GameObject(std::string tag);
		~GameObject();

		int GetLayer() const { return layer; }
		void SetLayer(int newLayer) { layer = newLayer; }

		bool IsActive() const { return isActive; }
		void SetActive(bool state) { isActive = state; }

		Transform& GetTransform() { return transform; }

		void setParent(GameObject parent) { transform.SetParent(&parent.GetTransform()); }

	protected:
		Transform transform;

		int layer;
		std::string tag;
		bool isActive;
	};
}