/**
 * @file   PresentRPass.cpp
 * @brief  See PresentRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "PresentRPass.h"

#include "GameTechRenderer.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "FrameBuffer.h"
#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

using namespace NCL;
using namespace CSC8503;

using namespace NCL::CSC8503;

PresentRPass::PresentRPass() : OGLPresentRenderPass() {
	quad = AssetLibrary<MeshGeometry>::GetAsset("quad");

	shader = AssetLoader::CreateShaderAndInit("present.vert", "present.frag");
}

PresentRPass::~PresentRPass() {
}

void PresentRPass::Render() {
	shader->Bind();

	sceneTexIn->Bind(0);

	quad->Draw();

	shader->Unbind();
}

void PresentRPass::SetGamma(float gamma) {
	shader->Bind();

	shader->SetUniformFloat("gamma", gamma);

	shader->Unbind();
}
