#pragma once
#include "../Common/Matrix4.h"
#include "../Common/MeshGeometry.h"

class ShaderBase;
class TextureBase;

namespace NCL {
	namespace Rendering {
		class ShaderBase;
		class TextureBase;
	}
	namespace PS4 {
		class RenderObject	{
		public:
			RenderObject();
			~RenderObject();

			RenderObject(CSC8503::MeshGeometry* m, Rendering::ShaderBase* shader, Rendering::TextureBase* tex);

			const Maths::Matrix4& GetLocalTransform() const;
			void	SetLocalTransform(const Maths::Matrix4& mat);

			Rendering::ShaderBase* GetShader() const {
				return shader;
			}

		//protected:
			CSC8503::MeshGeometry*	mesh;
			Rendering::ShaderBase*		shader;

			static const int TEXTURE_COUNT = 4;
			Rendering::TextureBase* textures[TEXTURE_COUNT];

			Maths::Matrix4		localTransform;
		};
	}
}
