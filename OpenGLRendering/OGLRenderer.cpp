/**
 * @file   OGLRenderer.cpp
 * @brief  See OGLRenderer.h
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "OGLRenderer.h"

#include "OGLBufferObject.h"
#include "OGLFrameBuffer.h"
#include "OGLMesh.h"
#include "OGLRenderPass.h"
#include "OGLShader.h"
#include "OGLTexture.h"

#include "AssetLoader.h"

#ifdef _WIN32
#include "Win32Window.h"

#include "glad\gl.h"
#include "KHR\khrplatform.h"
#include "KHR/WGLext.h"

PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
#endif

using namespace NCL;
using namespace NCL::Rendering;

#ifdef OPENGL_DEBUGGING
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
#endif;

OGLRenderer::OGLRenderer(Window& w) : RendererBase(w), config(*this) {
#ifdef _WIN32
	InitWithWin32(w);
#endif

	if (initState) {
		AssetLoader::RegisterMeshLoadFunction(OGLMesh::LoadMesh);
		AssetLoader::RegisterTextureLoadFunction(OGLTexture::LoadTexture);

		AssetLoader::RegisterBufferObjectCreateFunction(OGLBufferObject::CreateBufferObject);
		AssetLoader::RegisterFrameBufferCreateFunction(OGLFrameBuffer::CreateFrameBuffer);
		AssetLoader::RegisterMeshCreateFunction(OGLMesh::CreateMesh);
		AssetLoader::RegisterShaderCreateFunction(OGLShader::CreateShader);
		AssetLoader::RegisterTextureCreateFunction(OGLTexture::CreateTexture);
		AssetLoader::RegisterShaderCreateAndInitFunction(OGLShader::CreateShaderAndInit);
	}

	GetConfig().ResetAll();
}

OGLRenderer::~OGLRenderer() {
#ifdef _WIN32
	DestroyWithWin32();
#endif
}

void OGLRenderer::OnWindowResize(int width, int height) {
	RendererBase::OnWindowResize(width, height);

	GetConfig().SetViewport();
}

void OGLRenderer::ClearBuffers(ClearBit mask) {
	GLbitfield glmask;
	switch (mask) {
		default:
		case ClearBit::Color   : glmask = GL_COLOR_BUFFER_BIT  ; break;
		case ClearBit::Depth   : glmask = GL_DEPTH_BUFFER_BIT  ; break;
		case ClearBit::Stencil : glmask = GL_STENCIL_BUFFER_BIT; break;
		case ClearBit::ColorDepth   : glmask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  ; break;
		case ClearBit::ColorStencil : glmask = GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT; break;
		case ClearBit::DepthStencil : glmask = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT; break;
		case ClearBit::ColorDepthStencil: glmask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT; break;
	}
	glClear(glmask);
}

void OGLRenderer::BeginFrame() {
}

void OGLRenderer::EndFrame() {
}

void OGLRenderer::SwapBuffers() {
	::SwapBuffers(deviceContext);
}

void OGLRenderer::ClearBackbuffer() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

#ifdef _WIN32
void OGLRenderer::InitWithWin32(Window& w) {
	Win32Code::Win32Window* realWindow = (Win32Code::Win32Window*)&w;

	if (!(deviceContext = GetDC(realWindow->GetHandle()))) {
		std::cout << __FUNCTION__ << " Failed to create window!\n";
		return;
	}

	//A pixel format descriptor is a struct that tells the Windows OS what type of front / back buffers we want to create etc
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion     = 1;
	pfd.dwFlags      = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   //It must be double buffered, it must support OGL(!), and it must allow us to draw to it...
	pfd.iPixelType   = PFD_TYPE_RGBA; //We want our front / back buffer to have 4 channels!
	pfd.cColorBits   = 32; //4 channels of 8 bits each!
	pfd.cDepthBits   = 24; //24 bit depth buffer
	pfd.cStencilBits = 8; //plus an 8 bit stencil buffer
	pfd.iLayerType   = PFD_MAIN_PLANE;

	GLuint PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(deviceContext, &pfd))) { // Did Windows Find A Matching Pixel Format for our PFD?
		std::cout << __FUNCTION__ << " Failed to choose a pixel format!\n";
		return;
	}

	if (!SetPixelFormat(deviceContext, PixelFormat, &pfd)) { // Are We Able To Set The Pixel Format?
		std::cout << __FUNCTION__ << " Failed to set a pixel format!\n";
		return;
	}

	HGLRC tempContext; //We need a temporary OpenGL context to check for OpenGL 3.2 compatibility...stupid!!!
	if (!(tempContext = wglCreateContext(deviceContext))) { // Are We Able To get the temporary context?
		std::cout << __FUNCTION__ << " Cannot create a temporary context!\n";
		wglDeleteContext(tempContext);
		return;
	}

	if (!wglMakeCurrent(deviceContext, tempContext)) { // Try To Activate The Rendering Context
		std::cout << __FUNCTION__ << " Cannot set temporary context!\n";
		wglDeleteContext(tempContext);
		return;
	}
	if (!gladLoaderLoadGL()) {
		std::cout << __FUNCTION__ << " Cannot initialise GLAD!\n";
		return;
	}
	//Now we have a temporary context, we can find out if we support OGL 4.x
	char* ver = (char*)glGetString(GL_VERSION); // ver must equal "4.1.0" (or greater!)
	int major = ver[0] - '0'; //casts the 'correct' major version integer from our version string
	int minor = ver[2] - '0'; //casts the 'correct' minor version integer from our version string

	if (major < 3) {
		std::cout << __FUNCTION__ << " Device does not support OpenGL 4.x!\n";
		wglDeleteContext(tempContext);
		return;
	}

	if (major == 4 && minor < 1) {
		std::cout << __FUNCTION__ << " Device does not support OpenGL 4.1!\n";
		wglDeleteContext(tempContext);
		return;
	}

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#ifdef OPENGL_DEBUGGING 
		| WGL_CONTEXT_DEBUG_BIT_ARB
#endif //No deprecated stuff!! DIE DIE DIE glBegin!!!!
		,WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	//Everywhere else in the Renderers, we use function pointers provided by GLEW...but we can't initialise GLEW yet! So we have to use the 'Wiggle' API
	//to get a pointer to the function that will create our OpenGL 3.2 context...
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	renderContext = wglCreateContextAttribsARB(deviceContext, 0, attribs);

	// Check for the context, and try to make it the current rendering context
	if (!renderContext || !wglMakeCurrent(deviceContext, renderContext)) {
		std::cout << __FUNCTION__ << " Cannot set OpenGL 3 context!\n";
		wglDeleteContext(renderContext);
		wglDeleteContext(tempContext);
		return;
	}

	wglDeleteContext(tempContext);

	std::cout << __FUNCTION__ << " Initialised OpenGL " << major << "." << minor << " rendering context\n";

#ifdef OPENGL_DEBUGGING
	glDebugMessageCallback(DebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	initState = true; 

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	w.SetRenderer(this);
}

HGLRC OGLRenderer::CreateContext() {
	HGLRC context;
	if (!initState) return 0;

	char* ver = (char*)glGetString(GL_VERSION); // ver must equal "4.1.0" (or greater!)
	int major = ver[0] - '0'; //casts the 'correct' major version integer from our version string
	int minor = ver[2] - '0'; //casts the 'correct' minor version integer from our version string

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#ifdef OPENGL_DEBUGGING 
		| WGL_CONTEXT_DEBUG_BIT_ARB
#endif //No deprecated stuff!! DIE DIE DIE glBegin!!!!
		,WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0 //That's enough attributes...
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	context = wglCreateContextAttribsARB(deviceContext, 0, attribs);
	if (!wglShareLists(renderContext, context)) {
		wglDeleteContext(context);
		std::cout << "Error Sharing Between Files\n";
	}

	return context;
}


void OGLRenderer::DestroyWithWin32() {
	wglDeleteContext(renderContext);
}

bool OGLRenderer::SetVerticalSync(VerticalSyncState s) {
	if (!wglSwapIntervalEXT) {
		return false;
	}
	GLuint state;

	switch (s) {
		case VerticalSyncState::Off      : state =  0; break;
		default:
		case VerticalSyncState::On       : state =  1; break;
		case VerticalSyncState::Adaptive : state = -1; break;
	}

	return wglSwapIntervalEXT(state);
}
#endif

#ifdef OPENGL_DEBUGGING
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	std::string sourceName;
	std::string typeName;
	std::string severityName;

	switch (source) {
		case GL_DEBUG_SOURCE_API             : sourceName = "Source(OpenGL)"         ; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM   : sourceName = "Source(Window System)"  ; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER : sourceName = "Source(Shader Compiler)"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY     : sourceName = "Source(Third Party)"    ; break;
		case GL_DEBUG_SOURCE_APPLICATION     : sourceName = "Source(Application)"    ; break;
		case GL_DEBUG_SOURCE_OTHER           : sourceName = "Source(Other)"          ; break;
	}

	switch (type) {
		case GL_DEBUG_TYPE_ERROR               : typeName = "Type(Error)"               ; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR : typeName = "Type(Deprecated Behaviour)"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  : typeName = "Type(Undefined Behaviour)" ; break;
		case GL_DEBUG_TYPE_PORTABILITY         : typeName = "Type(Portability)"         ; break;
		case GL_DEBUG_TYPE_PERFORMANCE         : typeName = "Type(Performance)"         ; break;
		case GL_DEBUG_TYPE_OTHER               : typeName = "Type(Other)"               ; break;
	}

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH   : severityName = "Priority(High)"  ; break;
		case GL_DEBUG_SEVERITY_MEDIUM : severityName = "Priority(Medium)"; break;
		case GL_DEBUG_SEVERITY_LOW    : severityName = "Priority(Low)"   ; break;
	}

	std::cout << "OpenGL Debug Output: " + sourceName + ", " + typeName + ", " + severityName + ", " + std::string(message) << "\n";
}
#endif
