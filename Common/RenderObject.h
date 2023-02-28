#pragma once
#include "TextureBase.h"
#include "ShaderBase.h"
#include "Vector4.h"

namespace NCL {
	namespace Rendering {
		class OGLShader;
	}

	using namespace NCL::Rendering;

	
	namespace CSC8503 {
		class MeshGeometry;
		class Transform;
		using namespace Maths;

		class RenderObject
		{
		public:
			RenderObject(Transform* parentTransform, CSC8503::MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader = nullptr);
			RenderObject(RenderObject& other, Transform* parentTransform);
			~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				texture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return texture;
			}

			MeshGeometry* GetMesh() const {
				return mesh;
			}


			void SetTransform(Transform* transform) {
				this->transform = transform;
			}

			Transform* GetTransform() const {
				return transform;
			}

			ShaderBase* GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

			virtual void ConfigerShaderExtras(OGLShader* shaderOGL) const {}

		protected:
			Transform* transform;
			CSC8503::MeshGeometry* mesh;
			TextureBase* texture;
			ShaderBase* shader;
			Vector4			colour;
		};
	}
}
