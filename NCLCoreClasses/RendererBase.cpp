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
		pass.pass.WindowResize(splitWidth, splitHeight);
	}
	if (combinePass) {
		combinePass->WindowResize(splitWidth, splitHeight);
	}
	for (auto& pass : postRenderPasses) {
		pass.pass.WindowResize(splitWidth, splitHeight);
	}
	if (presentPass) {
		presentPass->WindowResize(splitWidth, splitHeight);
	}
	for (auto& pass : overlayRenderPasses) {
		pass.pass.WindowResize(splitWidth, splitHeight);
	}
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
	bool temp2 = overlayRenderPasses[overlayMap["Hud"]].enabled;
	for (size_t i = 0; i < numPlayers; i++) {
		size_t index = i * 4;
		GetConfig().SetDefaultViewport(0, 0, viewports[index + 2], viewports[index + 3]);
		SetGameWorldMainCamera(i);
		UpdateHudDisplay(i);
		RenderScene();
		GetConfig().SetDefaultViewport(viewports[index], viewports[index + 1], viewports[index + 2], viewports[index + 3]);
		RenderPresent();
		DisplayWinLoseInformation(i);
		EnableOverlayPass("Menu", false);
		EnableOverlayPass("Hud", true);
		EnableOverlayPass("Debug", true);
		UpdatePipeline();
		RenderOverlay();

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
