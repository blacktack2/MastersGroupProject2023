/**
 * @file   PS4Renderer.cpp
 * @brief  See PS4Renderer.h
 *
 * @author Daniel Abraham
 * @author Stuart Lewis
 * @date   February 2023
 */
#ifdef _ORBIS

#include "PS4Renderer.h"
#include "PS4Window.h"
#include <video_out.h>	//Video System
#include <gnmx\basegfxcontext.h>

#include <.\graphics\api_gnm\toolkit\allocators.h>
#include <.\graphics\api_gnm\toolkit\stack_allocator.h>



#include "PS4Mesh.h"
#include "PS4Shader.h"
#include "PS4Texture.h"

 //#include "PS4BufferObject.h"
//#include "PS4FrameBuffer.h"
//#include "PS4RenderPass.h"

#include "AssetLoader.h"

using namespace NCL;
using namespace NCL::Rendering;


PS4Renderer::PS4Renderer(Window& w) : RendererBase(w),
_MaxCMDBufferCount(3), _bufferCount(3), _GarlicMemory(1024 * 1024 * 512), _OnionMemory(1024 * 1024 * 256){
		framesSubmitted = 0;
		currentGPUBuffer = 0;
		currentScreenBuffer = 0;
		prevScreenBuffer = 0;

		//std::cerr << "Starting Rich Code!" << std::endl;

		currentGFXContext = nullptr;

		InitialiseMemoryAllocators();

		InitialiseGCMRendering();
		InitialiseVideoSystem();
		window->SetRenderer(this);
		SwapScreenBuffer();
		SwapCommandBuffer();//always swap at least once...

	if (initState) {
		AssetLoader::RegisterMeshLoadFunction(PS4Mesh::LoadMesh);
		AssetLoader::RegisterTextureLoadFunction(PS4Texture::LoadTexture);

		/*AssetLoader::RegisterBufferObjectCreateFunction(PS4BufferObject::CreateBufferObject);
		AssetLoader::RegisterFrameBufferCreateFunction(PS4FrameBuffer::CreateFrameBuffer);*/
		AssetLoader::RegisterMeshCreateFunction(PS4Mesh::CreateMesh);
		AssetLoader::RegisterShaderCreateFunction(PS4Shader::CreateShader);
		AssetLoader::RegisterTextureCreateFunction(PS4Texture::CreateTexture);
		AssetLoader::RegisterShaderCreateAndInitFunction(PS4Shader::CreateShaderAndInit);
	}

	//GetConfig().ResetAll();
}

PS4Renderer::~PS4Renderer() {
	DestroyGCMRendering();
	DestroyVideoSystem();
	DestroyMemoryAllocators();
}

void PS4Renderer::OnWindowResize(int width, int height) {
	RendererBase::OnWindowResize(width, height);

	GetConfig().SetViewport();
}

void PS4Renderer::ClearBuffers(ClearBit mask) {
	if ((int)mask & (int)ClearBit::Color) {
		//Vector4 defaultClearColour(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1.0f);
		SonyMath::Vector4 defaultClearColour(0.1f, 0.1f, 0.1f, 1.0f);
		SurfaceUtil::clearRenderTarget(*currentGFXContext, &currentPS4Buffer->colourTarget, defaultClearColour);
	}

	if ((int)mask & (int)ClearBit::Depth) {
		float defaultDepth = 1.0f;
		SurfaceUtil::clearDepthTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, defaultDepth);
	}

	if ((int)mask & (int)ClearBit::Stencil && currentPS4Buffer->depthTarget.getStencilReadAddress()) {
		int defaultStencil = 0;
		SurfaceUtil::clearStencilTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, defaultStencil);
	}
}

void PS4Renderer::BeginFrame() {
}

void PS4Renderer::EndFrame() {
	framesSubmitted++;
}

void PS4Renderer::SwapBuffers() {
	SwapScreenBuffer();
	SwapCommandBuffer();
}

void PS4Renderer::ClearBackbuffer() {
	ClearBuffers(ClearBit::ColorDepth);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

PS4ScreenBuffer* NCL::Rendering::PS4Renderer::GenerateScreenBuffer(uint width, uint height, bool colour, bool depth, bool stencil)
{
	PS4ScreenBuffer* buffer = new PS4ScreenBuffer();

	if (colour) {
		Gnm::RenderTargetSpec spec;
		spec.init();
		spec.m_width = width;
		spec.m_height = height;
		spec.m_numSamples = Gnm::kNumSamples1;
		spec.m_numFragments = Gnm::kNumFragments1;
		spec.m_colorFormat = Gnm::kDataFormatB8G8R8A8UnormSrgb;

		GpuAddress::computeSurfaceTileMode(Gnm::GpuMode::kGpuModeBase, &spec.m_colorTileModeHint, GpuAddress::kSurfaceTypeColorTargetDisplayable, spec.m_colorFormat, 1);

		int32_t success = buffer->colourTarget.init(&spec);

		if (success != SCE_GNM_OK) {
			bool a = true;
		}

		const Gnm::SizeAlign colourAlign = buffer->colourTarget.getColorSizeAlign();

		void* colourMemory = stackAllocators[MEMORY_GARLIC].allocate(colourAlign);

		Gnm::registerResource(nullptr, ownerHandle, colourMemory, colourAlign.m_size,
			"Colour", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);

		buffer->colourTarget.setAddresses(colourMemory, NULL, NULL);
	}

	if (depth) {
		Gnm::DepthRenderTargetSpec spec;
		spec.init();
		spec.m_width = width;
		spec.m_height = height;
		spec.m_numFragments = Gnm::kNumFragments1;
		spec.m_zFormat = Gnm::ZFormat::kZFormat32Float;
		spec.m_stencilFormat = (stencil ? Gnm::kStencil8 : Gnm::kStencilInvalid);

		GpuAddress::computeSurfaceTileMode(Gnm::GpuMode::kGpuModeBase, &spec.m_tileModeHint, GpuAddress::kSurfaceTypeDepthTarget, Gnm::DataFormat::build(spec.m_zFormat), 1);

		void* stencilMemory = 0;

		int32_t success = buffer->depthTarget.init(&spec);

		if (success != SCE_GNM_OK) {
			bool a = true;
		}

		void* depthMemory = stackAllocators[MEMORY_GARLIC].allocate(buffer->depthTarget.getZSizeAlign());

		Gnm::registerResource(nullptr, ownerHandle, depthMemory, buffer->depthTarget.getZSizeAlign().m_size,
			"Depth", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);

		if (stencil) {
			stencilMemory = stackAllocators[MEMORY_GARLIC].allocate(buffer->depthTarget.getStencilSizeAlign());

			Gnm::registerResource(nullptr, ownerHandle, stencilMemory, buffer->depthTarget.getStencilSizeAlign().m_size,
				"Stencil", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);
		}

		buffer->depthTarget.setAddresses(depthMemory, stencilMemory);
	}

	return buffer;
}

void NCL::Rendering::PS4Renderer::SwapScreenBuffer()
{
	prevScreenBuffer = currentScreenBuffer;
	currentScreenBuffer = (currentScreenBuffer + 1) % _bufferCount;
	sceVideoOutSubmitFlip(videoHandle, prevScreenBuffer, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, 0);

	currentPS4Buffer = screenBuffers[currentScreenBuffer];
}

void NCL::Rendering::PS4Renderer::SwapCommandBuffer()
{
	if (currentGFXContext) {
		if (currentGFXContext->submit() != sce::Gnm::kSubmissionSuccess) {
			std::cerr << "Graphics queue submission failed?" << std::endl;
		}
		Gnm::submitDone();
	}

	currentGPUBuffer = (currentGPUBuffer + 1) % _MaxCMDBufferCount;

	currentFrame = &frames[currentGPUBuffer];
	currentGFXContext = &currentFrame->GetCommandBuffer();
}

void NCL::Rendering::PS4Renderer::InitialiseMemoryAllocators(){

	stackAllocators[MEMORY_GARLIC].init(SCE_KERNEL_WC_GARLIC, _GarlicMemory);
	stackAllocators[MEMORY_ONION].init(SCE_KERNEL_WB_ONION, _OnionMemory);

	oAllocator = Gnmx::Toolkit::GetInterface(&stackAllocators[MEMORY_ONION]);
	gAllocator = Gnmx::Toolkit::GetInterface(&stackAllocators[MEMORY_GARLIC]);

	this->garlicAllocator = &gAllocator;
	this->onionAllocator = &oAllocator;
	Gnm::registerOwner(&ownerHandle, "PS4RendererBase");

}

void NCL::Rendering::PS4Renderer::InitialiseVideoSystem()
{
	screenBuffers = new PS4ScreenBuffer * [_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) {
		screenBuffers[i] = GenerateScreenBuffer(1920, 1080);
	}

	//Now we can open up the video port
	videoHandle = sceVideoOutOpen(0, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);

	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute,
		SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB,
		SCE_VIDEO_OUT_TILING_MODE_TILE,
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
		screenBuffers[0]->colourTarget.getWidth(),
		screenBuffers[0]->colourTarget.getHeight(),
		screenBuffers[0]->colourTarget.getPitch()
	);

	void* bufferAddresses[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) {
		bufferAddresses[i] = screenBuffers[i]->colourTarget.getBaseAddress();
	}

	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses, _bufferCount, &attribute);
}

void NCL::Rendering::PS4Renderer::InitialiseGCMRendering(){
	frames = (PS4Frame*)onionAllocator->allocate(sizeof(PS4Frame) * _MaxCMDBufferCount, alignof(PS4Frame));

	for (int i = 0; i < _MaxCMDBufferCount; ++i) {
		new (&frames[i])PS4Frame();
	}

	sce::Gnmx::Toolkit::Allocators allocators = sce::Gnmx::Toolkit::Allocators(oAllocator, gAllocator, ownerHandle);
	Gnmx::Toolkit::initializeWithAllocators(&allocators);
}

void NCL::Rendering::PS4Renderer::DestroyMemoryAllocators(){
	stackAllocators[MEMORY_GARLIC].deinit();
	stackAllocators[MEMORY_ONION].deinit();
}

void NCL::Rendering::PS4Renderer::DestroyVideoSystem(){
	for (int i = 0; i < _bufferCount; ++i) {
		delete screenBuffers[i];
	}
	delete[] screenBuffers;

	sceVideoOutClose(videoHandle);
}

void NCL::Rendering::PS4Renderer::DestroyGCMRendering(){

}
