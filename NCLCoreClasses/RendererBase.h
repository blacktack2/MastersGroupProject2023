/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "Window.h"

#include "RenderPassBase.h"
#include "IMainRenderPass.h"
#include "ICombineRenderPass.h"
#include "IPostRenderPass.h"
#include "IPresentRenderPass.h"
#include "IOverlayRenderPass.h"

#include <string>
#include <vector>

namespace NCL::Rendering {
	enum class VerticalSyncState {
		VSync_ON,
		VSync_OFF,
		VSync_ADAPTIVE
	};
	class RendererBase {
	public:
		friend class NCL::Window;

		RendererBase(Window& w);
		virtual ~RendererBase();

		virtual bool HasInitialised() const {return true;}

		virtual void Update(float dt) {}

		void Render() {
			BeginFrame();
			RenderFrame();
			EndFrame();
			SwapBuffers();
		}

		virtual bool SetVerticalSync(VerticalSyncState s) {
			return false;
		}

		void EnableRenderScene(bool enable);
		void EnablePostProcessing(bool enable);
		void EnableRenderOverlay(bool enable);

		void UpdatePipeline();

		inline float GetAspect() const {
			return (float)windowWidth / (float)windowHeight;
		}
		inline int GetWidth() const {
			return windowWidth;
		}
		inline int GetHeight() const {
			return windowHeight;
		}
	protected:
		inline void AddMainPass(IMainRenderPass* pass) {
			mainRenderPasses.emplace_back(pass);
		}
		inline void SetCombinePass(ICombineRenderPass* pass) {
			combinePass = pass;
		}
		inline void AddPostPass(IPostRenderPass* pass) {
			postRenderPasses.emplace_back(pass, true);
		}
		inline void SetPresentPass(IPresentRenderPass* pass) {
			presentPass = pass;
		}
		inline void AddOverlayPass(IOverlayRenderPass* pass) {
			overlayRenderPasses.emplace_back(pass, true);
		}

		virtual void OnWindowResize(int width, int height);
		virtual void OnWindowDetach() {};

		virtual void BeginFrame()  = 0;
		virtual void EndFrame()    = 0;
		virtual void SwapBuffers() = 0;
		virtual void ClearBackbuffer() = 0;
		Window& hostWindow;

		int windowWidth;
		int windowHeight;
	private:
		struct MainPass {
			IMainRenderPass* pass;
		};
		struct PostPass {
			IPostRenderPass* pass;
			bool enabled;
		};
		struct OverPass {
			IOverlayRenderPass* pass;
			bool enabled;
		};

		void RenderFrame();

		bool doRenderScene = true;
		bool doRenderPost  = true;
		bool doRenderOver  = true;

		std::vector<MainPass> mainRenderPasses;
		ICombineRenderPass* combinePass;
		std::vector<PostPass> postRenderPasses;
		IPresentRenderPass* presentPass;
		std::vector<OverPass> overlayRenderPasses;

		std::vector<IRenderPass*> renderPipeline;
	};
}