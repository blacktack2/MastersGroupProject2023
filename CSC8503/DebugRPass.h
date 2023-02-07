/**
 * @file   DebugRPass.h
 * @brief
 *
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLRenderPass.h"

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class DebugRPass : public OGLRenderPass {
	public:
		DebugRPass(OGLRenderer& renderer);
		~DebugRPass();

		virtual void Render() override;
	};
}
