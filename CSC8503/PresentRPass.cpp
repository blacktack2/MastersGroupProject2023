/**
 * @file   PresentRPass.cpp
 * @brief  See PresentRPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "PresentRPass.h"

#include "AssetLibrary.h"
#include "AssetLoader.h"

#include "MeshGeometry.h"
#include "ShaderBase.h"
#include "TextureBase.h"

using namespace NCL;
using namespace NCL::CSC8503;
using namespace NCL::Rendering;

PresentRPass::PresentRPass() : OGLPresentRenderPass() {
	quad = AssetLibrary<MeshGeometry>::GetAsset("quad");

	shader = AssetLoader::CreateShaderAndInit("present.vert", "present.frag");
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
