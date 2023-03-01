/**
 * @file   RendererBase.h
 * @brief  Base rendering class for handling general updates and pipeline
 * organisation.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "Window.h"

#include "IMainRenderPass.h"
#include "ICombineRenderPass.h"
#include "IPostRenderPass.h"
#include "IPresentRenderPass.h"
#include "IOverlayRenderPass.h"
#include "RenderPassBase.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace NCL::Rendering {
	enum class VerticalSyncState {
		On,
		Off,
		Adaptive
	};

	enum class ClearBit {
		Color,
		Depth,
		Stencil,
		ColorDepth,
		ColorStencil,
		DepthStencil,
		ColorDepthStencil,
	};

	class RendererConfigBase;

	/**
	 * @brief Base rendering class for handling general updates and pipeline
	 * organisation.
	 */
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

		/**
		 * @brief Enable or disable general scene rendering and post-processing.
		 * Must be followed by a call to UpdatePipeline() to take effect.
		 */
		void EnableRenderScene(bool enable);
		/**
		 * @brief Enable or disable post-processing. Must be followed by a call to
		 * UpdatePipeline() to take effect.
		 */
		void EnablePostProcessing(bool enable);
		/**
		 * @brief Enable or disable the overlay passes. Must be followed by a call
		 * to UpdatePipeline() to take effect.
		 */
		void EnableRenderOverlay(bool enable);

		/**
		 * @brief Enable or disable specific post-processing pass. Must be followed
		 * by a call to UpdatePipeline() to take effect.
		 * 
		 * @param name Name of the pass defined during the initial call to
		 * AddPostPass().
		 * @param enable If true enable the pass, otherwise disable it.
		 */
		void EnablePostPass(const std::string& name, bool enable);
		/**
		 * @brief Enable or disable specific overlay pass. Must be followed
		 * by a call to UpdatePipeline() to take effect.
		 * 
		 * @param name Name of the pass defined during the initial call to
		 * AddOverlayPass().
		 * @param enable If true enable the pass, otherwise disable it.
		 */
		void EnableOverlayPass(const std::string& name, bool enable);

		/**
		 * @brief Update the render pipeline to match the state defined by the
		 * pipeline setter functions.
		 */
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

		virtual void ClearBuffers(ClearBit mask) = 0;
		virtual RendererConfigBase& GetConfig() = 0;
	protected:
		/**
		 * @brief Add main render pass. Meant to add to, or modify, the GBuffer
		 * in preparation for the combine stage.
		 * @brief When enabled, this stage will run first, and is followed by
		 * the combine pass.
		 * @brief Enabled passes are called in the same order they are added
		 * using this method.
		 * @brief Must be followed by a call to UpdatePipeline() to take effect.
		 */
		inline void AddMainPass(IMainRenderPass* pass) {
			mainRenderPasses.emplace_back(pass);
		}
		/**
		 * @brief Set the combine render pass. Meant to take elements of the
		 * GBuffer defined during the main pass, and combine them into a single
		 * texture.
		 * @brief When general scene rendering is enabled, this will be
		 * executed after the final main pass, and before the first
		 * post-processing or overlay pass.
		 * @brief Must be followed by a call to UpdatePipeline() to take effect.
		 */
		inline void SetCombinePass(ICombineRenderPass* pass) {
			combinePass = pass;
		}
		/**
		 * @brief Add post-processing render pass. Meant to take in a screen
		 * size texture and output a new texture with a post-processing effect
		 * applied.
		 * @brief When enabled, this stage will run after the combine pass (and
		 * main stage), and is followed by the presentation pass.
		 * @brief Enabled passes are called in the same order they are added
		 * using this method.
		 * @brief Must be followed by a call to UpdatePipeline() to take effect.
		 * 
		 * @param name Unique name identifier for this render pass.
		 */
		inline void AddPostPass(IPostRenderPass* pass, const std::string& name) {
			postRenderPasses.emplace_back(pass, true);
			postMap.insert({ name, postRenderPasses.size() - 1 });
		}
		/**
		 * @brief Set the presentation render pass. Meant to take a texture,
		 * either from post-processing or directly from the combine pass, and
		 * draw it to the backbuffer. Will always be called after clearing the
		 * backbuffer.
		 * Gamma correction may take place here.
		 * @brief When general scene rendering is enabled, this will execute
		 * after the last post-processing or main pass, and before the first
		 * overlay pass.
		 * @brief Must be followed by a call to UpdatePipeline() to take
		 * effect.
		 */
		inline void SetPresentPass(IPresentRenderPass* pass) {
			presentPass = pass;
		}
		/**
		 * @brief Add overlay render pass. Meant to draw over top the contents
		 * of the backbuffer (primarily for HUD and debug passes).
		 * @brief When enabled, this stage will take place after the presentation
		 * pass, unless general scene rendering is disabled in which case this will
		 * be the only stage to run.
		 * @brief Enabled passes are called in the same order they are added
		 * using this method.
		 * @brief Must be followed by a call to UpdatePipeline() to take effect.
		 * 
		 * @param name Unique name identifier for this render pass.
		 */
		inline void AddOverlayPass(IOverlayRenderPass* pass, const std::string& name) {
			overlayRenderPasses.emplace_back(pass, true);
			overlayMap.insert({ name, overlayRenderPasses.size() - 1 });
		}

		virtual void OnWindowResize(int width, int height);
		virtual void OnWindowDetach() {};

		/**
		 * @brief Update method called before executing the render pipeline.
		 */
		virtual void BeginFrame()  = 0;
		/**
		 * @brief Update method called after executing the render pipeline.
		 * 
		 */
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
		ICombineRenderPass*   combinePass = nullptr;
		std::vector<PostPass> postRenderPasses;
		IPresentRenderPass*   presentPass = nullptr;
		std::vector<OverPass> overlayRenderPasses;

		std::unordered_map<std::string, size_t> postMap;
		std::unordered_map<std::string, size_t> overlayMap;

		std::vector<IRenderPass*> renderPipeline;
	};
}
