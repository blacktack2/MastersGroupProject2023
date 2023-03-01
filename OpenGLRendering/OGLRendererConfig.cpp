/**
 * @file   OGLRendererConfig.cpp
 * @brief  See OGLRendererConfig.h.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "OGLRendererConfig.h"

#include "glad\gl.h"

#include <unordered_map>

using namespace NCL;
using namespace Rendering;

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
