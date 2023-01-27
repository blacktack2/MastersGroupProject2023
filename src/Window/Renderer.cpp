/**
 * @file   Renderer.cpp
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   January 2023
 */
#include "Renderer.h"

#include <glad/gl.h>
#include <glad/include/KHR/khrplatform.h>
#include <KHR/wglext.h>

using namespace Graphics;

PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

#ifdef OPENGL_DEBUGGING
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif

Renderer::Renderer(Application::Window& window) : mWindow(window) {
	if (!(mDeviceContext = GetDC(mWindow.getHandle())))
		return;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion     = 1;
	pfd.dwFlags      = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType   = PFD_TYPE_RGBA;
	pfd.cColorBits   = 32;
	pfd.cDepthBits   = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType   = PFD_MAIN_PLANE;

	GLuint pixelFormat;
	if (!(pixelFormat = ChoosePixelFormat(mDeviceContext, &pfd)))
		return;

	if (!SetPixelFormat(mDeviceContext, pixelFormat, &pfd))
		return;

	HGLRC tempContext;
	if (!(tempContext = wglCreateContext(mDeviceContext))) {
		wglDeleteContext(tempContext);
		return;
	}

	if (!wglMakeCurrent(mDeviceContext, tempContext)) {
		wglDeleteContext(tempContext);
		return;
	}

	if (!gladLoaderLoadGL())
		return;

	char* ver = (char*)glGetString(GL_VERSION);
	int major = ver[0] - '0';
	int minor = ver[2] - '0';

	if (major < 3) {
		wglDeleteContext(tempContext);
		return;
	}

	if (major == 4 && minor < 1) {
		wglDeleteContext(tempContext);
		return;
	}

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#ifdef OPENGL_DEBUGGING
		| WGL_CONTEXT_DEBUG_BIT_ARB
#endif
		, WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	auto wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	mRenderContext = wglCreateContextAttribsARB(mDeviceContext, 0, attribs);

	if (!mRenderContext || !wglMakeCurrent(mDeviceContext, mRenderContext)) {
		wglDeleteContext(mRenderContext);
		wglDeleteContext(tempContext);
		return;
	}

	wglDeleteContext(tempContext);

	glEnable(GL_FRAMEBUFFER_SRGB);

#ifdef OPENGL_DEBUGGING
	glDebugMessageCallback(DebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	glClearColor(0, 0, 0, 1);

	window.setRenderer(this);
	mInitSuccess = true;
}

Renderer::~Renderer() {
	wglDeleteContext(mRenderContext);
}

void Renderer::swapBuffers() {
	::SwapBuffers(mDeviceContext);
}

void Renderer::onWindowResize(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
}

void Renderer::onWindowDetach() {
}

static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string sourceName;
	std::string typeName;
	std::string severityName;

	switch (source) {
		case GL_DEBUG_SOURCE_API             : sourceName = "OpenGL"         ; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM   : sourceName = "Window System"  ; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER : sourceName = "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY     : sourceName = "Third Party"    ; break;
		case GL_DEBUG_SOURCE_APPLICATION     : sourceName = "Application"    ; break;
		case GL_DEBUG_SOURCE_OTHER           : sourceName = "Other"          ; break;
	}
	switch (type) {
		case GL_DEBUG_TYPE_ERROR               : typeName = "Error"               ; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR : typeName = "Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  : typeName = "Undefined Behaviour" ; break;
		case GL_DEBUG_TYPE_PORTABILITY         : typeName = "Portability"         ; break;
		case GL_DEBUG_TYPE_PERFORMANCE         : typeName = "Performance"         ; break;
		case GL_DEBUG_TYPE_OTHER               : typeName = "Other"               ; break;
	}
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH   : severityName = "High"  ; break;
		case GL_DEBUG_SEVERITY_MEDIUM : severityName = "Medium"; break;
		case GL_DEBUG_SEVERITY_LOW    : severityName = "Low"   ; break;
	}

	std::string m =
		std::string("OpenGL Debug Output: Source(").append(sourceName)
		.append("), Type(").append(typeName)
		.append("), Severity(").append(severityName).append(")");
}
