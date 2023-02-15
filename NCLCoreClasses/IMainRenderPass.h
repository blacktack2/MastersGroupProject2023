/**
 * @file   IMainRenderPass.h
 * @brief  Interface class for main stage render passes.
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once

namespace NCL::Rendering {
	class TextureBase;

	class IMainRenderPass {
	public:
		virtual void OnWindowResize(int width, int height) = 0;
		virtual void Render() = 0;
	};
}
