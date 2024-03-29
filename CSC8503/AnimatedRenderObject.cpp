/**
 * @file   AnimatedRenderObject.cpp
 * @brief  See AnimatedRenderObject.h.
 * 
 * @author Xiaoyang Liu
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "AnimatedRenderObject.h"

#include "GameWorld.h"

#include "AssetLibrary.h"

#include "MeshAnimation.h"
#include "MeshGeometry.h"
#include "MeshMaterial.h"
#include "ShaderBase.h"

using namespace NCL;
using namespace CSC8503;
using namespace Rendering;

AnimatedRenderObject::AnimatedRenderObject(Transform& parentTransform, std::shared_ptr<MeshGeometry> mesh, std::shared_ptr<MeshMaterial> material, std::shared_ptr<MeshAnimation> animation) :
RenderObject(parentTransform, mesh, material), anim(animation) {
	currentFrame = 0;
	nextFrame = 0;
	frameTime = 0.0f;
}

AnimatedRenderObject::~AnimatedRenderObject() {
}

void AnimatedRenderObject::PreDraw(int sublayer, ShaderBase& shader) {
	frameTime -= GameWorld::instance().GetDeltaTime() / GameWorld::instance().GetScreenNum() * animSpeed;
	while (frameTime < 0.0f) {
		currentFrame = nextFrame++;
		nextFrame    = nextFrame == anim->GetFrameCount() ? 0 : nextFrame;
		frameTime += 1.0f / anim->GetFrameRate();
	}

	//const std::vector<Matrix4>& bindPose    = mesh->GetBindPose();
	const std::vector<Matrix4>& invBindPose = GetMesh()->GetInverseBindPose();
	const std::vector<Matrix4>  frameData   = anim->GetJointData(currentFrame);
	//const std::vector<int>& bindPoseIndices = mesh->GetBindPoseIndices();

	std::vector<Matrix4> frameMatrices;
	for (unsigned int i = 0; i < std::min(GetMesh()->GetJointCount(), anim->GetJointCount()); i++) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}
	shader.SetUniformMatrix("joints", frameMatrices);
	//animSpeed = 1.0f;
}

void AnimatedRenderObject::PreShadow(int sublayer, ShaderBase& shader) {
	const std::vector<Matrix4>& invBindPose = GetMesh()->GetInverseBindPose();
	const std::vector<Matrix4>  frameData = anim->GetJointData(currentFrame);

	std::vector<Matrix4> frameMatrices;
	for (unsigned int i = 0; i < std::min(GetMesh()->GetJointCount(), anim->GetJointCount()); i++) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}
	shader.SetUniformMatrix("joints", frameMatrices);
}



ShaderBase& AnimatedRenderObject::GetDefaultShader() {
	return *AssetLibrary<ShaderBase>::GetAsset("animationDefault");
}

ShaderBase& AnimatedRenderObject::GetDefaultShadowShader() {
	return *AssetLibrary<ShaderBase>::GetAsset("shadowAnimated");
}