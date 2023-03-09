/**
 * @file   RendererConfigBase.h
 * @brief  Base wrapper class for global renderer settings.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
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

	/**
	 * @brief Base wrapper class for global renderer settings.
	 */
	class RendererConfigBase {
	public:
		RendererConfigBase(RendererBase& renderer);

		/**
		 * @brief Set all settings to their default values.
		 * @brief This is a potentially expensive operation, so should not be called
		 * during the mainloop.
		 */
		void ResetAll();

		/**
		 * @brief Set the bounds of the viewport.
		 * @brief Determines the pixel range in which NDC space is set to
		 * match.
		 */
		virtual void SetViewport(int x, int y, unsigned int width, unsigned int height) = 0;
		/**
		 * @brief Set the bounds of the viewport.
		 * @brief This overload resets the viewport to fit the current bounds of the window.
		 */
		inline void SetViewport() {
			SetViewport(0, 0, renderer.GetWidth(), renderer.GetHeight());
		}
		/**
		 * @brief Set the colour to clear buffers to.
		 * @brief See RendererBase::ClearBuffers().
		 */
		virtual void SetClearColour(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f) = 0;
		/**
		 * @brief Configure settings for alpha testing.
		 * @brief Alpha test discards fragments based on the comparison between an
		 * incoming fragment's alpha value, and a constant reference value.
		 * 
		 * @param enabled If true enable alpha testing, otherwise disable.
		 * @param func    Alpha comparison function.
		 * @param ref     Reference value that incoming alpha values are compared to (clamped 0-1).
		 */
		virtual void SetAlphaTest(bool enabled = false, AlphaTestFunc func = AlphaTestFunc::Default, float ref = ALPHA_TEST_REF_DEFAULT) = 0;
		/**
		 * @brief Configure settings for blending.
		 * @brief Blending defines how to combine incoming (source) rgba values
		 * with existing (destination) values in the frame buffer.
		 * 
		 * @param enabled   If true enable blending, otherwise disable.
		 * @param srcFactor How the rgba source blending factors are scaled.
		 * @param dstFactor How the rgba destination blending factors are scaled.
		 */
		virtual void SetBlend(bool enabled = false, BlendFuncSrc srcFactor = BlendFuncSrc::Default, BlendFuncDst dstFactor = BlendFuncDst::Default) = 0;
		/**
		 * @brief Configure settings for colour masking.
		 * @brief Enable or disable specific colour components (rgba) from
		 * being written to.
		 */
		virtual void SetColourMask(bool red = true, bool green = true, bool blue = true, bool alpha = true) = 0;
		/**
		 * @brief Configure settings for facet culling.
		 * @brief Facet culling determines whether front/back facing facets
		 * should be drawn.
		 * 
		 * @param enabled If true enable facet culling, otherwise disable.
		 * @param mode    Which faces should be culled.
		 */
		virtual void SetCullFace(bool enabled = true, CullFace mode = CullFace::Default) = 0;
		/**
		 * @brief Configure settings for depth testing.
		 * @brief Depth testing performs depth comparisons between fragments,
		 * and updates the depth buffer.
		 * 
		 * @param enabled If true enable depth testing, otherwise disable.
		 * @param func    Depth comparison function.
		 * @param nearVal Mapping of the near clipping plane.
		 * @param farVal  Mapping fo the far clipping plane.
		 */
		virtual void SetDepthTest(bool enabled = false, DepthTestFunc func = DepthTestFunc::Default, float nearVal = 0.0f, float farVal = 1.0f) = 0;
		/**
		 * @brief Enable or disable writing to the depth buffer.
		 */
		virtual void SetDepthMask(bool enabled = false) = 0;
	private:
		RendererBase& renderer;
	};
}
