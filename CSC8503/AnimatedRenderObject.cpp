/**
 * @file   AnimatedRenderObject.cpp
 * @brief  See AnimatedRenderObject.h.
 * 
 * @author Xiaoyang Liu
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "AnimatedRenderObject.h"

#include "AssetLibrary.h"
#include "Assets.h"
#include "GameWorld.h"
#include "OGLShader.h"

using namespace NCL::CSC8503;

AnimatedRenderObject::AnimatedRenderObject(Transform* parentTransform, MeshGeometry* mesh, MeshMaterial* material, MeshAnimation* animation) :
RenderObject(parentTransform, mesh, material) {
	anim = animation;

	currentFrame = 0;
	nextFrame = 0;
	frameTime = 0.0f;
}

AnimatedRenderObject::~AnimatedRenderObject() {
	delete anim;
}

void AnimatedRenderObject::PreDraw(int sublayer, ShaderBase* shader) {
	frameTime -= GameWorld::instance().GetDeltaTime() * animSpeed;
	while (frameTime < 0.0f) {
		currentFrame = nextFrame++;
		nextFrame    = nextFrame == anim->GetFrameCount() ? 0 : nextFrame;
		frameTime += 1.0f / anim->GetFrameRate();
	}

	const Matrix4* bindPose = mesh->GetBindPose().data();
	const Matrix4* invBindPose = mesh->GetInverseBindPose().data();
	const Matrix4* frameData = anim->GetJointData(currentFrame);
	const int* bindPoseIndices = mesh->GetBindPoseIndices();

	std::vector<Matrix4> frameMatrices;
	for (unsigned int i = 0; i < mesh->GetJointCount(); i++) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}
	shader->SetUniformMatrix("joints", frameMatrices.size(), frameMatrices.data());
}

ShaderBase* AnimatedRenderObject::GetDefaultShader() {
	return AssetLibrary::GetShader("animationDefault");
}
