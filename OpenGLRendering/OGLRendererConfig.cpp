/**
 * @file   OGLRendererConfig.cpp
 * @brief  See OGLRendererConfig.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "OGLRendererConfig.h"

#include "glad\gl.h"

using namespace NCL::Rendering;

OGLRendererConfig::OGLRendererConfig(RendererBase& renderer) : RendererConfigBase(renderer) {
}

void OGLRendererConfig::SetViewport(int x, int y, unsigned int width, unsigned int height) {
	glViewport(x, y, width, height);
}

void OGLRendererConfig::SetClearColour(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

void OGLRendererConfig::SetAlphaTest(bool enabled, AlphaTestFunc func, float ref) {
	if (enabled) {
		GLenum glfunc;
		switch (func) {
			case AlphaTestFunc::Never        : glfunc = GL_NEVER   ; break;
			case AlphaTestFunc::Less         : glfunc = GL_LESS    ; break;
			case AlphaTestFunc::Equal        : glfunc = GL_EQUAL   ; break;
			case AlphaTestFunc::LessEqual    : glfunc = GL_LEQUAL  ; break;
			case AlphaTestFunc::NotEqual     : glfunc = GL_NOTEQUAL; break;
			case AlphaTestFunc::GreaterEqual : glfunc = GL_GEQUAL  ; break;
			default:
			case AlphaTestFunc::Always       : glfunc = GL_ALWAYS  ; break;
		}
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(glfunc, ref);
	} else {
		glDisable(GL_ALPHA_TEST);
	}
}

void OGLRendererConfig::SetBlend(bool enabled, BlendFuncSrc srcFactor, BlendFuncDst dstFactor) {
	if (enabled) {
		GLenum glsrcFactor, gldstFactor;
		switch (srcFactor) {
			case BlendFuncSrc::Zero                   : glsrcFactor = GL_ZERO                    ; break;
			default:
			case BlendFuncSrc::One                    : glsrcFactor = GL_ONE                     ; break;
			case BlendFuncSrc::SrcColour              : glsrcFactor = GL_SRC_COLOR               ; break;
			case BlendFuncSrc::OnMinusSrcColour       : glsrcFactor = GL_ONE_MINUS_SRC_COLOR     ; break;
			case BlendFuncSrc::DstColour              : glsrcFactor = GL_DST_COLOR               ; break;
			case BlendFuncSrc::OneMinusDstColour      : glsrcFactor = GL_ONE_MINUS_DST_COLOR     ; break;
			case BlendFuncSrc::SrcAlpha               : glsrcFactor = GL_SRC_ALPHA               ; break;
			case BlendFuncSrc::OneMinusSrcAlpha       : glsrcFactor = GL_ONE_MINUS_SRC_ALPHA     ; break;
			case BlendFuncSrc::DstAlpha               : glsrcFactor = GL_DST_ALPHA               ; break;
			case BlendFuncSrc::OneMinusDstAlpha       : glsrcFactor = GL_ONE_MINUS_DST_ALPHA     ; break;
			case BlendFuncSrc::ConstantColour         : glsrcFactor = GL_CONSTANT_COLOR          ; break;
			case BlendFuncSrc::OneMinusConstantColour : glsrcFactor = GL_ONE_MINUS_CONSTANT_COLOR; break;
			case BlendFuncSrc::ConstantAlpha          : glsrcFactor = GL_CONSTANT_ALPHA          ; break;
			case BlendFuncSrc::OneMinusConstantAlpha  : glsrcFactor = GL_ONE_MINUS_CONSTANT_ALPHA; break;
			case BlendFuncSrc::SrcAlphaSaturate       : glsrcFactor = GL_SRC_ALPHA_SATURATE      ; break;
		}
		switch (dstFactor) {
			default:
			case BlendFuncDst::Zero                   : gldstFactor = GL_ZERO                    ; break;
			case BlendFuncDst::One                    : gldstFactor = GL_ONE                     ; break;
			case BlendFuncDst::SrcColour              : gldstFactor = GL_SRC_COLOR               ; break;
			case BlendFuncDst::OnMinusSrcColour       : gldstFactor = GL_ONE_MINUS_SRC_COLOR     ; break;
			case BlendFuncDst::DstColour              : gldstFactor = GL_DST_COLOR               ; break;
			case BlendFuncDst::OneMinusDstColour      : gldstFactor = GL_ONE_MINUS_DST_COLOR     ; break;
			case BlendFuncDst::SrcAlpha               : gldstFactor = GL_SRC_ALPHA               ; break;
			case BlendFuncDst::OneMinusSrcAlpha       : gldstFactor = GL_ONE_MINUS_SRC_ALPHA     ; break;
			case BlendFuncDst::DstAlpha               : gldstFactor = GL_DST_ALPHA               ; break;
			case BlendFuncDst::OneMinusDstAlpha       : gldstFactor = GL_ONE_MINUS_DST_ALPHA     ; break;
			case BlendFuncDst::ConstantColour         : gldstFactor = GL_CONSTANT_COLOR          ; break;
			case BlendFuncDst::OneMinusConstantColour : gldstFactor = GL_ONE_MINUS_CONSTANT_COLOR; break;
			case BlendFuncDst::ConstantAlpha          : gldstFactor = GL_CONSTANT_ALPHA          ; break;
			case BlendFuncDst::OneMinusConstantAlpha  : gldstFactor = GL_ONE_MINUS_CONSTANT_ALPHA; break;
		}
		glEnable(GL_BLEND);
		glBlendFunc(glsrcFactor, gldstFactor);
	} else {
		glDisable(GL_BLEND);
	}
}

void OGLRendererConfig::SetColourMask(bool red, bool green, bool blue, bool alpha) {
	glColorMask(red, green, blue, alpha);
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

void OGLRendererConfig::SetDepthTest(bool enabled, DepthTestFunc func, float nearVal, float farVal) {
	if (enabled) {
		GLenum glfunc;
		switch (func) {
			case DepthTestFunc::Never        : glfunc = GL_NEVER; break;
			default:
			case DepthTestFunc::Less         : glfunc = GL_LESS; break;
			case DepthTestFunc::Equal        : glfunc = GL_EQUAL; break;
			case DepthTestFunc::LessEqual    : glfunc = GL_LEQUAL; break;
			case DepthTestFunc::Greater      : glfunc = GL_GREATER; break;
			case DepthTestFunc::NotEqual     : glfunc = GL_NOTEQUAL; break;
			case DepthTestFunc::GreaterEqual : glfunc = GL_GEQUAL; break;
			case DepthTestFunc::Always       : glfunc = GL_ALWAYS; break;
		}
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(glfunc);
		glDepthRange(nearVal, farVal);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void OGLRendererConfig::SetDepthMask(bool enabled) {
	glDepthMask(enabled);
}
