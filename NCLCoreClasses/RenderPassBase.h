/**
 * @file   RenderPassBase.h
 * @brief  Base class for render passes containing functionality common to all
 * interfaces.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "TextureBase.h"

#include <functional>
#include <vector>

namespace NCL::Rendering {
	/**
	 * @brief Base class for render passes containing functionality common to
	 * all interfaces.
	 */
	class RenderPassBase {
	public:
		virtual ~RenderPassBase();

		virtual void WindowResize(int width, int height);

		virtual void Render() = 0;
	protected:
		RenderPassBase();

		virtual void OnWindowResize() {}

		void AddScreenTexture(TextureBase& tex);
	private:
		std::vector<std::reference_wrapper<TextureBase>> textures;
	};
}
