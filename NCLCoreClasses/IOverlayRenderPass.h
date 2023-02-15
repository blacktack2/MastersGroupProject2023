/**
 * @file   IOverlayRenderPass.h
 * @brief  Interface class for overlay render passes (writing directly to the
 * backbuffer after post-processing).
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once

namespace NCL::Rendering {
	class TextureBase;

	class IOverlayRenderPass {
	public:
		virtual void OnWindowResize(int width, int height) = 0;
		virtual void Render() = 0;
	};
}
