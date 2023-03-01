/**
 * @file   OGLRendererConfig.cpp
 * @brief  See OGLRendererConfig.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "OGLRendererConfig.h"

#include "glad\gl.h"

using namespace NCL;
using namespace Rendering;

OGLRendererConfig::OGLRendererConfig(RendererBase& renderer) : RendererConfigBase(renderer) {
}

void OGLRendererConfig::SetViewport(int x, int y, unsigned int width, unsigned int height) {
	glViewport(0, 0, width, height);
}

void OGLRendererConfig::SetAlphaTest(bool enabled, AlphaTestFunc func, float ref) {
	if (enabled) {
		GLenum glFunc;
		switch (func) {
			case AlphaTestFunc::Never        : glFunc = GL_NEVER   ; break;
			case AlphaTestFunc::Less         : glFunc = GL_LESS    ; break;
			case AlphaTestFunc::Equal        : glFunc = GL_EQUAL   ; break;
			case AlphaTestFunc::LessEqual    : glFunc = GL_LEQUAL  ; break;
			case AlphaTestFunc::NotEqual     : glFunc = GL_NOTEQUAL; break;
			case AlphaTestFunc::GreaterEqual : glFunc = GL_GEQUAL  ; break;
			default:
			case AlphaTestFunc::Always       : glFunc = GL_ALWAYS  ; break;
		}
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(glFunc, ref);
	} else {
		glDisable(GL_ALPHA_TEST);
	}
}

void OGLRendererConfig::SetBlend(bool enabled, BlendFuncSrc srcFactor, BlendFuncDst dstFactor) {
	if (enabled) {
		GLenum sfactor, dfactor;
		switch (srcFactor) {
			case BlendFuncSrc::Zero                   : sfactor = GL_ZERO                    ; break;
			default:
			case BlendFuncSrc::One                    : sfactor = GL_ONE                     ; break;
			case BlendFuncSrc::SrcColour              : sfactor = GL_SRC_COLOR               ; break;
			case BlendFuncSrc::OnMinusSrcColour       : sfactor = GL_ONE_MINUS_SRC_COLOR     ; break;
			case BlendFuncSrc::DstColour              : sfactor = GL_DST_COLOR               ; break;
			case BlendFuncSrc::OneMinusDstColour      : sfactor = GL_ONE_MINUS_DST_COLOR     ; break;
			case BlendFuncSrc::SrcAlpha               : sfactor = GL_SRC_ALPHA               ; break;
			case BlendFuncSrc::OneMinusSrcAlpha       : sfactor = GL_ONE_MINUS_SRC_ALPHA     ; break;
			case BlendFuncSrc::DstAlpha               : sfactor = GL_DST_ALPHA               ; break;
			case BlendFuncSrc::OneMinusDstAlpha       : sfactor = GL_ONE_MINUS_DST_ALPHA     ; break;
			case BlendFuncSrc::ConstantColour         : sfactor = GL_CONSTANT_COLOR          ; break;
			case BlendFuncSrc::OneMinusConstantColour : sfactor = GL_ONE_MINUS_CONSTANT_COLOR; break;
			case BlendFuncSrc::ConstantAlpha          : sfactor = GL_CONSTANT_ALPHA          ; break;
			case BlendFuncSrc::OneMinusConstantAlpha  : sfactor = GL_ONE_MINUS_CONSTANT_ALPHA; break;
			case BlendFuncSrc::SrcAlphaSaturate       : sfactor = GL_SRC_ALPHA_SATURATE      ; break;
		}
		switch (dstFactor) {
			default:
			case BlendFuncDst::Zero                   : dfactor = GL_ZERO                    ; break;
			case BlendFuncDst::One                    : dfactor = GL_ONE                     ; break;
			case BlendFuncDst::SrcColour              : dfactor = GL_SRC_COLOR               ; break;
			case BlendFuncDst::OnMinusSrcColour       : dfactor = GL_ONE_MINUS_SRC_COLOR     ; break;
			case BlendFuncDst::DstColour              : dfactor = GL_DST_COLOR               ; break;
			case BlendFuncDst::OneMinusDstColour      : dfactor = GL_ONE_MINUS_DST_COLOR     ; break;
			case BlendFuncDst::SrcAlpha               : dfactor = GL_SRC_ALPHA               ; break;
			case BlendFuncDst::OneMinusSrcAlpha       : dfactor = GL_ONE_MINUS_SRC_ALPHA     ; break;
			case BlendFuncDst::DstAlpha               : dfactor = GL_DST_ALPHA               ; break;
			case BlendFuncDst::OneMinusDstAlpha       : dfactor = GL_ONE_MINUS_DST_ALPHA     ; break;
			case BlendFuncDst::ConstantColour         : dfactor = GL_CONSTANT_COLOR          ; break;
			case BlendFuncDst::OneMinusConstantColour : dfactor = GL_ONE_MINUS_CONSTANT_COLOR; break;
			case BlendFuncDst::ConstantAlpha          : dfactor = GL_CONSTANT_ALPHA          ; break;
			case BlendFuncDst::OneMinusConstantAlpha  : dfactor = GL_ONE_MINUS_CONSTANT_ALPHA; break;
		}
		glEnable(GL_BLEND);
		glBlendFunc(sfactor, dfactor);
	} else {
		glDisable(GL_BLEND);
	}
}

void OGLRendererConfig::SetCullFace(bool enabled, CullFace mode) {
	if (enabled) {
		GLenum glmode;
		switch (mode) {
			case CullFace::Front        : glmode = GL_FRONT         ; break;
			default:
			case CullFace::Back         : glmode = GL_BACK          ; break;
			case CullFace::FrontAndBack : glmode = GL_FRONT_AND_BACK; break;
		}
		glEnable(GL_CULL_FACE);
		glCullFace(glmode);
	} else {
		glDisable(GL_CULL_FACE);
	}
}
