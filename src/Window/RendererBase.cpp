/**
 * @file   RendererBase.cpp
 * @brief  See RendererBase.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   January 2023
 */
#include "RendererBase.h"

#include "Window.h"

#include "../Debugging/Logger.h"

#include <glad/gl.h>
#include <glad/include/KHR/khrplatform.h>
#include <KHR/wglext.h>

using namespace Graphics;
using namespace Application;

PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

#ifdef OPENGL_DEBUGGING
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif

RendererBase::RendererBase() {
	if (!(mDeviceContext = GetDC(Window::getWindow().getHandle()))) {
		Debug::Logger::getLogger().fatal("Error getting device context.", __FILE__, __LINE__);
		return;
	}

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
	if (!(pixelFormat = ChoosePixelFormat(mDeviceContext, &pfd))) {
		Debug::Logger::getLogger().fatal("Error choosing pixel format.", __FILE__, __LINE__);
		return;
	}

	if (!SetPixelFormat(mDeviceContext, pixelFormat, &pfd)) {
		Debug::Logger::getLogger().fatal("Error setting pixel format.", __FILE__, __LINE__);
		return;
	}
	Debug::Logger::getLogger().trace("Pixel format set.");

	HGLRC tempContext;
	if (!(tempContext = wglCreateContext(mDeviceContext))) {
		Debug::Logger::getLogger().fatal("Error creating wgl device context.", __FILE__, __LINE__);
		wglDeleteContext(tempContext);
		return;
	}

	if (!wglMakeCurrent(mDeviceContext, tempContext)) {
		Debug::Logger::getLogger().fatal("Error setting temporary device context.", __FILE__, __LINE__);
		wglDeleteContext(tempContext);
		return;
	}
	Debug::Logger::getLogger().trace("Temporary device context created.");

	if (!gladLoaderLoadGL()) {
		Debug::Logger::getLogger().fatal("Error initializing OpenGL.", __FILE__, __LINE__);
		return;
	}
	Debug::Logger::getLogger().trace("OpenGL loaded");

	char* ver = (char*)glGetString(GL_VERSION);
	int major = ver[0] - '0';
	int minor = ver[2] - '0';

	if (major < 3) {
		Debug::Logger::getLogger().fatal("Incompatible OpenGL major version (must be 3+).", __FILE__, __LINE__);
		wglDeleteContext(tempContext);
		return;
	}

	if (major == 4 && minor < 1) {
		Debug::Logger::getLogger().fatal("Incompatible OpenGL minor version.", __FILE__, __LINE__);
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
		Debug::Logger::getLogger().fatal("Error setting main device context.", __FILE__, __LINE__);
		wglDeleteContext(mRenderContext);
		wglDeleteContext(tempContext);
		return;
	}
	Debug::Logger::getLogger().trace("Main device context created");

	wglDeleteContext(tempContext);

	glEnable(GL_FRAMEBUFFER_SRGB);

#ifdef OPENGL_DEBUGGING
	glDebugMessageCallback(DebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	glClearColor(0, 0, 0, 1);

	Window::getWindow().setRenderer(this);
	mInitSuccess = true;
}

RendererBase::~RendererBase() {
	wglDeleteContext(mRenderContext);
}

bool RendererBase::setVSync(VSyncState state) {
	return wglSwapIntervalEXT((int)state);
}

void RendererBase::swapBuffers() {
	::SwapBuffers(mDeviceContext);
}

void RendererBase::onWindowResize() {
	glViewport(0, 0, Window::getWindow().getWidth(), Window::getWindow().getHeight());
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
