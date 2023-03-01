/**
 * @file   RendererConfigBase.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "RendererBase.h"

namespace NCL::Rendering {
	enum class AlphaTestFunc {
		Never,
		Less,
		Equal,
		LessEqual,
		NotEqual,
		GreaterEqual,
		Always,
		Default = Always
	};
	constexpr float ALPHA_TEST_REF_DEFAULT = 0.0f;

	enum class BlendFuncSrc {
		Zero,
		One,
		SrcColour,
		OnMinusSrcColour,
		DstColour,
		OneMinusDstColour,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		ConstantColour,
		OneMinusConstantColour,
		ConstantAlpha,
		OneMinusConstantAlpha,
		SrcAlphaSaturate,
		Default = One,
	};
	enum class BlendFuncDst {
		Zero,
		One,
		SrcColour,
		OnMinusSrcColour,
		DstColour,
		OneMinusDstColour,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		ConstantColour,
		OneMinusConstantColour,
		ConstantAlpha,
		OneMinusConstantAlpha,
		Default = Zero
	};

	enum class CullFace {
		Front,
		Back,
		FrontAndBack,
		Default = Back
	};

	enum class DepthTestFunc {
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always,
		Default = Less
	};

	class RendererConfigBase {
	public:
		RendererConfigBase(RendererBase& renderer);

		void ResetAll();

		virtual void SetViewport(int x, int y, unsigned int width, unsigned int height) = 0;
		inline void SetViewport() {
			SetViewport(0, 0, renderer.GetWidth(), renderer.GetHeight());
		}
		virtual void SetClearColour(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f) = 0;
		virtual void SetAlphaTest(bool enabled = false, AlphaTestFunc func = AlphaTestFunc::Default, float ref = ALPHA_TEST_REF_DEFAULT) = 0;
		virtual void SetBlend(bool enabled = false, BlendFuncSrc srcFactor = BlendFuncSrc::Default, BlendFuncDst dstFactor = BlendFuncDst::Default) = 0;
		virtual void SetCullFace(bool enabled = true, CullFace mode = CullFace::Default) = 0;
		virtual void SetDepthTest(bool enabled = false, DepthTestFunc func = DepthTestFunc::Default, float nearVal = 0, float farVal = 1) = 0;
		virtual void SetDepthMask(bool enabled = false) = 0;
	private:
		RendererBase& renderer;
	};
}
