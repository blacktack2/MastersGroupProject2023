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

	class RendererConfigBase {
	public:
		RendererConfigBase(RendererBase& renderer);

		void ResetAll();

		virtual void SetViewport(int x, int y, unsigned int width, unsigned int height) = 0;
		inline void ResetViewport() {
			SetViewport(0, 0, renderer.GetWidth(), renderer.GetHeight());
		}

		virtual void SetAlphaTest(bool enabled, AlphaTestFunc func = AlphaTestFunc::Default, float ref = ALPHA_TEST_REF_DEFAULT) = 0;
		inline void ResetAlphaTest() {
			SetAlphaTest(false);
		}

		virtual void SetBlend(bool enabled, BlendFuncSrc srcFactor = BlendFuncSrc::Default, BlendFuncDst dstFactor = BlendFuncDst::Default) = 0;
		inline void ResetBlend() {
			SetBlend(false);
		}
	private:
		RendererBase& renderer;
	};
}
