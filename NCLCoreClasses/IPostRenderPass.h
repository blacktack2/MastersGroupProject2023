/**
 * @file   IPostRenderPass.h
 * @brief  Interface class for post-processing render passes.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once

namespace NCL::Rendering {
	class TextureBase;

	class IPostRenderPass {
	public:
		virtual void OnWindowResize(int width, int height) = 0;
		virtual void Render() = 0;

		virtual void SetSceneTexIn(TextureBase* sceneTex) = 0;
		virtual TextureBase* GetOutTex() const = 0;
	};
}
