/**
 * @file   RendererBase.cpp
 * @brief  See RendererBase.h.
 * 
 * @author Stuart Lewis
 * @author Xiaoyang Liu
 * @author Rich Davidson
 * @date   March 2023
 */
#include "RendererBase.h"

#include "RendererConfigBase.h"

#include <optional>

using namespace NCL;
using namespace Rendering;

RendererBase::RendererBase(Window& window) : hostWindow(window)	{
	windowWidth  = static_cast<int>(window.GetScreenSize().x);
	windowHeight = static_cast<int>(window.GetScreenSize().y);
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
			IPostRenderPass* last = nullptr;
			for (auto pass = postRenderPasses.begin(); pass != postRenderPasses.end(); pass++) {
				if (!pass->enabled) {
					continue;
				}
				if (last) {
					pass->pass.SetSceneTexIn(last->GetOutTex());
				} else {
					pass->pass.SetSceneTexIn(combinePass->GetOutTex());
				}
				renderPipeline.push_back(pass->pass);
				last = &pass->pass;
			}
			presentPass->SetSceneTexIn(last ? last->GetOutTex() : combinePass->GetOutTex());
		} else {
			presentPass->SetSceneTexIn(combinePass->GetOutTex());
		}
	}
	overlayPipeline.clear();
	if (doRenderOver) {
		for (auto& pass : overlayRenderPasses) {
			if (pass.enabled) {
				overlayPipeline.push_back(pass.pass);
			}
		}
	}
}

void RendererBase::OnWindowResize(int width, int height) {
	ResizeViewport();
}

void RendererBase::ResizeViewport() {
	switch (numPlayers) {
	default:
	case 1:
		splitWidth = GetWidth(); splitHeight = GetHeight(); break;
	case 2:
		splitWidth = GetWidth() * 0.5f; splitHeight = GetHeight(); break;
	case 3: case 4:
		splitWidth = GetWidth() * 0.5f; splitHeight = GetHeight() * 0.5f; break;
	}
	for (auto& pass : mainRenderPasses) {
		pass.pass.OnWindowResize(splitWidth, splitHeight);
	}
	if (combinePass) {
		combinePass->OnWindowResize(splitWidth, splitHeight);
	}
	for (auto& pass : postRenderPasses) {
		pass.pass.OnWindowResize(splitWidth, splitHeight);
	}
	if (presentPass) {
		presentPass->OnWindowResize(splitWidth, splitHeight);
	}
	for (auto& pass : overlayRenderPasses) {
		pass.pass.OnWindowResize(splitWidth, splitHeight);
	}
}

void RendererBase::RenderViewPort(int viewportID, int cameraID, const std::vector<float>& viewports, bool displayHudDebug)
{
	size_t index = viewportID * 4;
	GetConfig().SetDefaultViewport(0, 0, viewports[index + 2], viewports[index + 3]);
	SetGameWorldMainCamera(cameraID);
	RenderScene();
	GetConfig().SetDefaultViewport(viewports[index], viewports[index + 1], viewports[index + 2], viewports[index + 3]);
	RenderPresent();
	//DisplayWinLoseInformation(i);
	EnableOverlayPass("Menu", false);
	if (!displayHudDebug) {
		EnableOverlayPass("Hud", true);
		EnableOverlayPass("Debug", true);
	}
	UpdatePipeline();
	RenderOverlay();
}

void RendererBase::RenderFrame() {
	static const std::vector<float> viewports1 = {
		0.0f, 0.0f, 1.0f, 1.0f,
	};
	static const std::vector<float> viewports2 = {
		0.0f, 0.0f, 0.5f, 1.0f,
		0.5f, 0.0f, 0.5f, 1.0f,
	};
	static const std::vector<float> viewports4 = {
		0.0f, 0.0f, 0.5f, 0.5f,
		0.0f, 0.5f, 0.5f, 0.5f,
		0.5f, 0.0f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f, 0.5f,
	};
	const std::vector<float>& viewports = numPlayers > 2 ? viewports4 : (numPlayers > 1 ? viewports2 : viewports1);
	ClearBackbuffer();
	int mainCamera = GetGameWorldMainCamera();
	bool temp1 = overlayRenderPasses[overlayMap["Menu"]].enabled;

	//viewport 0 is always main camera
	RenderViewPort(0, mainCamera, viewports, temp1);
	int cameraID = 0;
	for (size_t i = 1; i < numPlayers; i++) {
		if (cameraID == mainCamera) {
			cameraID++;
		}
		//if (i > 1) { SetGameWorldDeltaTime(0.0f); }
		RenderViewPort(i, cameraID, viewports, temp1);
		cameraID++;
	}
	
	SetGameWorldMainCamera(mainCamera);
	GetConfig().SetDefaultViewport();
	EnableOverlayPass("Menu", temp1);
	EnableOverlayPass("Hud", false);
	EnableOverlayPass("Debug", false);
	UpdatePipeline();
	RenderOverlay();
}

void RendererBase::RenderScene() {
	for (auto& pass : renderPipeline) {
		pass.get().Render();
	}
}

void RendererBase::RenderPresent() {
	presentPass->Render();
}

void RendererBase::RenderOverlay() {
	for (auto& pass : overlayPipeline) {
		pass.get().Render();
	}
}
