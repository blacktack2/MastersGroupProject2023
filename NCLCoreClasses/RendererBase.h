/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "Window.h"

#include "IMainRenderPass.h"
#include "IPostRenderPass.h"
#include "IOverlayRenderPass.h"

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
			mainRenderPasses.push_back(pass);
		}
		inline void AddPostPass(IPostRenderPass* pass) {
			postRenderPasses.push_back(pass);
		}
		inline void AddOverlayPass(IOverlayRenderPass* pass) {
			overlayRenderPasses.push_back(pass);
		}

		virtual void OnWindowResize(int width, int height);
		virtual void OnWindowDetach() {};

		virtual void BeginFrame()  = 0;
		virtual void EndFrame()    = 0;
		virtual void SwapBuffers() = 0;
		Window& hostWindow;

		int windowWidth;
		int windowHeight;
	private:
		void RenderFrame();

		std::vector<IMainRenderPass*>  mainRenderPasses;
		std::vector<IPostRenderPass*> postRenderPasses;
		std::vector<IOverlayRenderPass*>  overlayRenderPasses;
	};
}