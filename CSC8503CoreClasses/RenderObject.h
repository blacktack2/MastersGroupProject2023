#pragma once
#include "TextureBase.h"
#include "ShaderBase.h"

namespace NCL {
	namespace Rendering {
		class OGLShader;
	}

	using namespace NCL::Rendering;

	class MeshGeometry;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, ShaderBase* shader);		// for RenderObject that hasAnimation
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader);
			RenderObject(RenderObject& other, Transform* parentTransform);
			~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture;
			}

			MeshGeometry*	GetMesh() const {
				return mesh;
			}


			void SetTransform(Transform* transform){
				this->transform = transform;
			}

			Transform*		GetTransform() const {
				return transform;
			}

			ShaderBase*		GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

			virtual void ConfigerShaderExtras(OGLShader* shaderOGL) const {}

			bool HasAnimation() const
			{
				return hasAnimation;
			}

		protected:
			Transform*		transform;
			MeshGeometry*	mesh;
			TextureBase*	texture;
			ShaderBase*		shader;
			Vector4			colour;
			bool			hasAnimation = false;
		};
	}
}
