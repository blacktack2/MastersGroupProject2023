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

#include "IMainRenderPass.h"
#include "ICombineRenderPass.h"
#include "IPostRenderPass.h"
#include "IPresentRenderPass.h"
#include "IOverlayRenderPass.h"
#include "RenderPassBase.h"

#include <optional>

using namespace NCL;
using namespace Rendering;

RendererBase::RendererBase(Window& window) : hostWindow(window)	{
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
	renderOverlaySplit.clear();
	renderOverlayFull.clear();
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
	if (doRenderOver) {
		for (auto& pass : overlayRenderPasses) {
			if (pass.enabled) {
				if (pass.split) {
					renderOverlaySplit.push_back(pass.pass);
				} else {
					renderOverlayFull.push_back(pass.pass);
				}
			}
		}
	}
}

void RendererBase::OnWindowResize(int width, int height) {
	ResizeViewport();
}

void RendererBase::ResizeViewport() {
	switch (numSplits) {
		default:
		case 1:
			splitWidth  = static_cast<int>(GetWidth());
			splitHeight = static_cast<int>(GetHeight());
			playerViewports[0] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
			break;
		case 2:
			splitWidth  = static_cast<int>(GetWidth() * 0.5f);
			splitHeight = static_cast<int>(GetHeight());
			playerViewports[0] = Vector4(0.0f, 0.0f, 0.5f, 1.0f);
			playerViewports[1] = Vector4(0.5f, 0.0f, 0.5f, 1.0f);
			break;
		case 4:
			playerViewports[3] = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
			[[ fallthrough ]];
		case 3:
			splitWidth  = static_cast<int>(GetWidth() * 0.5f);
			splitHeight = static_cast<int>(GetHeight() * 0.5f);
			playerViewports[0] = Vector4(0.0f, 0.0f, 0.5f, 0.5f);
			playerViewports[1] = Vector4(0.5f, 0.0f, 0.5f, 0.5f);
			playerViewports[2] = Vector4(0.0f, 0.5f, 0.5f, 0.5f);
			break;
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

void RendererBase::RenderViewPort(int cameraID, const Vector4& viewport) {
	GetConfig().SetDefaultViewport(0, 0, viewport.z, viewport.w);

	SetGameWorldMainCamera(cameraID);
	RenderScene();

	GetConfig().SetDefaultViewport(viewport.x, viewport.y, viewport.z, viewport.w);

	RenderPresent();
	RenderOverlaySplit();
	//DisplayWinLoseInformation(cameraID);
}

void RendererBase::RenderFrame() {
	ClearBackbuffer();
	int mainCamera = GetGameWorldMainCamera();
	bool temp1 = overlayRenderPasses[overlayMap["Menu"]].enabled;

	currentlySplit = true;
	RenderViewPort(mainCamera, playerViewports[0]);
	// Only time numSplits is > 1, is split-screen where mainCamera is always 0
	// In networking numSplits should always be 1, therefore this loop is ignored
	for (int cameraID = 1; cameraID < numSplits; cameraID++) {
		RenderViewPort(cameraID, playerViewports[cameraID]);
	}
	
	SetGameWorldMainCamera(mainCamera);
	GetConfig().SetDefaultViewport();
	currentlySplit = false;
	RenderOverlayFull();
}

void RendererBase::RenderScene() {
	for (auto& pass : renderPipeline) {
		pass.get().Render();
	}
}

void RendererBase::RenderPresent() {
	presentPass->Render();
}

void RendererBase::RenderOverlaySplit() {
	for (auto& pass : renderOverlaySplit) {
		pass.get().Render();
	}
}

void RendererBase::RenderOverlayFull() {
	for (auto& pass : renderOverlayFull) {
		pass.get().Render();
	}
}
