/**
 * @file   RendererBase.cpp
 * @brief  See RendererBase.h.
 * 
 * @author Stuart Lewis
 * @author Rich Davidson
 * @date   February 2023
 */
#include "RendererBase.h"

#include <optional>

using namespace NCL;
using namespace Rendering;

RendererBase::RendererBase(Window& window) : hostWindow(window)	{
	windowWidth  = window.GetScreenSize().x;
	windowHeight = window.GetScreenSize().y;
}

RendererBase::~RendererBase() {
}

void RendererBase::EnableRenderScene(bool enable) {
	doRenderScene = enable;
}

void RendererBase::EnablePostProcessing(bool enable) {
	doRenderPost = enable;
}

void RendererBase::EnableRenderOverlay(bool enable) {
	doRenderOver = enable;
}

void RendererBase::EnablePostPass(const std::string& name, bool enable) {
	auto pass = postMap.find(name);
	if (pass == postMap.end()) {
		return;
	}
	postRenderPasses[pass->second].enabled = enable;
}

void RendererBase::EnableOverlayPass(const std::string& name, bool enable) {
	auto pass = overlayMap.find(name);
	if (pass == overlayMap.end()) {
		return;
	}
	overlayRenderPasses[pass->second].enabled = enable;
}

void RendererBase::UpdatePipeline() {
	renderPipeline.clear();
	if (doRenderScene) {
		for (auto& pass : mainRenderPasses) {
			renderPipeline.push_back(pass.pass);
		}
		renderPipeline.push_back(*combinePass);
		if (doRenderPost) {
			bool first = true;
			std::optional<std::reference_wrapper<IPostRenderPass>> last;
			for (auto pass = postRenderPasses.begin(); pass != postRenderPasses.end(); pass++) {
				if (!pass->enabled) {
					continue;
				}
				if (first) {
					first = false;
					pass->pass.SetSceneTexIn(combinePass.value().get().GetOutTex());
				} else {
					pass->pass.SetSceneTexIn(last.value().get().GetOutTex());
				}
				renderPipeline.push_back(pass->pass);
				last = pass->pass;
			}
			presentPass.value().get().SetSceneTexIn(last.has_value() ? last.value().get().GetOutTex() : combinePass.value().get().GetOutTex());
		} else {
			presentPass.value().get().SetSceneTexIn(combinePass.value().get().GetOutTex());
		}
		renderPipeline.push_back(*presentPass);
	}
	if (doRenderOver) {
		for (auto& pass : overlayRenderPasses) {
			if (pass.enabled) {
				renderPipeline.push_back(pass.pass);
			}
		}
	}
}

void RendererBase::OnWindowResize(int width, int height) {
	for (auto& pass : mainRenderPasses) {
		pass.pass.OnWindowResize(width, height);
	}
	if (combinePass) {
		combinePass.value().get().OnWindowResize(width, height);
	}
	for (auto& pass : postRenderPasses) {
		pass.pass.OnWindowResize(width, height);
	}
	if (presentPass) {
		presentPass.value().get().OnWindowResize(width, height);
	}
	for (auto& pass : overlayRenderPasses) {
		pass.pass.OnWindowResize(width, height);
	}
}

void RendererBase::RenderFrame() {
	ClearBackbuffer();
	for (auto& pass : renderPipeline) {
		pass.get().Render();
	}
}
