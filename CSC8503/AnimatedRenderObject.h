#pragma once
#include "RenderObject.h"
#include "OGLTexture.h"
#include <vector>
#include "MeshAnimation.h"
#include "MeshGeometry.h"
#include "MeshMaterial.h"

namespace NCL::CSC8503 {
	class AnimatedRenderObject : public RenderObject {
	public:
		AnimatedRenderObject(MeshMaterial* mater, MeshAnimation* ani, MeshGeometry* msh, std::vector<OGLTexture*>  matTex, Transform* parentTransform);
		~AnimatedRenderObject();

		void ConfigerShaderExtras(OGLShader* shaderOGL) const override;

		MeshAnimation* GetAnimation() const {
			return anim;
		}

		void SetAnimation(MeshAnimation* ani) {
			currentFrame = 0;
			anim = ani;
		}

		MeshGeometry* GetAnimatedMesh() const {
			return animMesh;
		}

		int GetCurrentFrame() const {
			return currentFrame;
		}

		void SetCurrentFrame(int frame) {
			currentFrame = frame;
		}

		float GetFrameTime() {
			return frameTime;
		}

		void SetFrameTime(float t) {
			frameTime = t;
		}

		void SetNextFrame(int frame) {
			nextFrame = frame;
		}

		std::vector<OGLTexture*> GetMatTextures() {
			return matTextures;
		}

		float GetAnimSpeed() {
			return animSpeed;
		}

		void SetAnimSpeed(float s) {
			animSpeed = s;
		}

	protected:
		MeshMaterial* material = nullptr;
		MeshAnimation* anim = nullptr;
		MeshGeometry* animMesh = nullptr;

		std::vector<OGLTexture*>  matTextures;

		int		currentFrame;
		int		nextFrame;

		float	frameTime;
		float	animSpeed = 1.0f;
	};
}