/**
 * @file   OGLRenderPass.cpp
 * @brief  See OGLRenderPass.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "OGLRenderPass.h"

using namespace NCL::Rendering;

OGLRenderPass::OGLRenderPass(OGLRenderer& renderer) : RenderPassBase(), renderer(renderer) {
}

OGLRenderPass::~OGLRenderPass() {
}