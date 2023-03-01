/**
 * @file   OGLRendererConfig.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RendererConfigBase.h"

namespace NCL::Rendering {
	class OGLRendererConfig : public RendererConfigBase {
	public:
		void SetAlphaTest(bool enabled, AlphaTestFunc func = AlphaTestFunc::Default, float ref = ALPHA_TEST_REF_DEFAULT) override;
	};
}
