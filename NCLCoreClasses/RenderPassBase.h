/**
 * @file   RenderPassBase.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "TextureBase.h"

#include <vector>

namespace NCL::Rendering {
	class RenderPassBase {
	public:
		virtual ~RenderPassBase();

		virtual void OnWindowResize(int width, int height);

		virtual void Render() = 0;
	protected:
		RenderPassBase();

		void AddScreenTexture(TextureBase* tex);
	private:
		std::vector<TextureBase*> textures;
	};
}
