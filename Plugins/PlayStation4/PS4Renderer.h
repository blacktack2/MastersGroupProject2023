/**
 * @file   PS4Renderer.h
 * @brief  Base Playstation 4 implementation of the main renderer.
 *
 * @author Stuart Lewis
 * @author Daniel Abraham
 * @date   February 2023
 */
#pragma once
#ifdef _ORBIS
#include "RendererBase.h"

//#include "OGLRendererConfig.h"

#include "PS4MemoryAware.h"

#include <gnm.h>
#include <gnmx\fetchshaderhelper.h>

#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h>
#include "PS4Frame.h"
#include "PS4Texture.h"

using namespace sce;
using namespace Gnmx;
using namespace Gnmx::Toolkit;

namespace NCL::Maths {
	class Matrix4;
}

namespace NCL {

	namespace Rendering {
		enum MemoryLocation {
			MEMORY_GARLIC,
			MEMORY_ONION,
			MEMORY_MAX
		};

		struct PS4ScreenBuffer {
			sce::Gnm::RenderTarget		colourTarget;
			sce::Gnm::DepthRenderTarget depthTarget;
		};
		/**
		 * @brief Base Playstation 4 implementation of the main renderer.
		 */
		class PS4Renderer : public RendererBase, public PS4::PS4MemoryAware {
		public:
			/*friend class PS4LoadingManager;*/
			PS4Renderer(Window& w);
			~PS4Renderer();

			void OnWindowResize(int width, int height) override;
			bool HasInitialised() const override {
				return initState;
			}

			void ForceValidDebugState(bool newState) {
				forceValidDebugState = newState;
			}

			virtual bool SetVerticalSync(VerticalSyncState s) override;

			Gnmx::GnmxGfxContext& GetCurrentGFXContext() { return &currentGFXContext; }

			/*RendererConfigBase& GetConfig() override {
				return config;
			}*/

			void ClearBuffers(ClearBit mask) override;
		protected:
			void BeginFrame() override;
			void EndFrame() override;

			void SwapBuffers() override;
			void ClearBackbuffer() override;


			//void	SetRenderBuffer(PS4ScreenBuffer* buffer, bool clearColour, bool clearDepth, bool clearStencil);
			//void	ClearBuffer(bool colour, bool depth, bool stencil);
			PS4ScreenBuffer* GenerateScreenBuffer(uint width, uint height, bool colour = true, bool depth = true, bool stencil = false);

		private:
			//OGLRendererConfig config;

			void	SwapScreenBuffer();
			void	SwapCommandBuffer();

			void	InitialiseMemoryAllocators();
			void	InitialiseVideoSystem();
			void	InitialiseGCMRendering();

			void	DestroyMemoryAllocators();
			void	DestroyVideoSystem();
			void	DestroyGCMRendering();

			bool initState = false;
			bool forceValidDebugState = false;

			int currentGPUBuffer;

			const int _MaxCMDBufferCount;

			//VIDEO SYSTEM VARIABLES
			int videoHandle;		//Handle to video system

		//SCREEN BUFFER VARIABLES
			const int			_bufferCount;	//How many screen buffers should we have
			int					currentScreenBuffer;
			int					prevScreenBuffer;
			PS4ScreenBuffer** screenBuffers;	//Pointer to our screen buffers
		//Memory Allocation
			const int _GarlicMemory;
			const int _OnionMemory;

			sce::Gnmx::Toolkit::StackAllocator	stackAllocators[MEMORY_MAX];

			//Individual Frames
			PS4::PS4Frame* frames;

			int framesSubmitted;

			//Per frame pointers...
			PS4ScreenBuffer* currentPS4Buffer;  //Pointer to whichever buffer we're currently using...
			Gnmx::GnmxGfxContext* currentGFXContext;
			PS4::PS4Frame* currentFrame;
		};
	}
}
#endif //_ORBIS