/**
 * @file   RenderPassBase.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once

namespace NCL::Rendering {
	class RenderPassBase {
	public:
		virtual ~RenderPassBase();

		virtual void Render() = 0;
		virtual void PreRender() = 0;
	protected:
		RenderPassBase();
	};
}
