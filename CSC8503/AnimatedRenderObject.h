/**
 * @file   AnimatedRenderObject.h
 * @brief  
 * 
 * @author Xiaoyang Liu
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RenderObject.h"

#include "OGLTexture.h"
#include "MeshAnimation.h"
#include "MeshGeometry.h"
#include "MeshMaterial.h"

#include <vector>

namespace NCL::CSC8503 {
	class AnimatedRenderObject : public RenderObject {
	public:
		AnimatedRenderObject(Transform* parentTransform, MeshGeometry* mesh, MeshMaterial* material, MeshAnimation* animation);
		~AnimatedRenderObject();

		inline const MeshAnimation* GetAnimation() const {
			return anim;
		}
		inline void SetAnimation(MeshAnimation* animation) {
			anim = animation;
		}
	protected:
		void PreDraw(int sublayer, ShaderBase* shader) override;

		ShaderBase* GetDefaultShader() override;

		MeshAnimation* anim = nullptr;

		int		currentFrame;
		int		nextFrame;

		float	frameTime;
		float	animSpeed = 1.0f;
	};
}
