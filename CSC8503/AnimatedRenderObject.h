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

#include <functional>
#include <memory>
#include <vector>

namespace NCL {
	class MeshAnimation;
	class MeshGeometry;
	class MeshMaterial;
}

namespace NCL {
	namespace Rendering {
		class ShaderBase;
	}
}

namespace NCL {
	namespace CSC8503 {
		class AnimatedRenderObject : public RenderObject {
		public:
			AnimatedRenderObject(Transform& parentTransform, std::shared_ptr<MeshGeometry> mesh, std::shared_ptr<MeshMaterial> material, std::shared_ptr<MeshAnimation> animation);
			~AnimatedRenderObject();

			inline const MeshAnimation& GetAnimation() const {
				return *anim;
			}
			inline void SetAnimation(std::shared_ptr<MeshAnimation> animation) {
				anim = animation;
				currentFrame = 0;
				nextFrame = 0;
			}
			inline void SetAniSpeed(float speed) {
				animSpeed = speed;
			}
		protected:
			void PreDraw(int sublayer, ShaderBase& shader) override;

			void PreShadow(int sublayer, ShaderBase& shader) override;

			ShaderBase& GetDefaultShader() override;

			std::shared_ptr<MeshAnimation> anim;

			ShaderBase& GetDefaultShadowShader() override;

			int currentFrame;
			int nextFrame;

			float frameTime;
			float animSpeed = 1.0f;
		};
	}
}
