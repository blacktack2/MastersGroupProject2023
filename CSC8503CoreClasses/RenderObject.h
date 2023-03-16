/**
 * @file   RenderObject.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include <functional>
#include <optional>
#include<Vector2.h>
#include<Vector4.h>
namespace NCL {
	class MeshMaterial;
	class MeshGeometry;
}

namespace NCL {
	namespace Rendering {
		class TextureBase;
		class ShaderBase;
	}
}

using namespace NCL::Rendering;

namespace NCL {
	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject {
		public:
			RenderObject(Transform& parentTransform, std::shared_ptr<MeshGeometry> mesh, std::shared_ptr<MeshMaterial> material);
			RenderObject(RenderObject& other, Transform& parentTransform);
			virtual ~RenderObject();

			std::shared_ptr<MeshGeometry> GetMesh() const {
				return mesh;
			}

			void SetTransform(Transform& transform) {
				this->transform = transform;
			}

			Transform& GetTransform() const {
				return transform;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			const Vector4& GetColour() const {
				return colour;
			}
			void SetTexScale(const Vector2& scale) {
				texScale = scale;
			}
			void DrawToGBuffer();
			void DrawToShadowMap();
		protected:
			virtual void PreDraw(int sublayer);
			virtual void PreDraw(int sublayer, ShaderBase& shader) {}

			virtual void PreShadow(int sublayer);
			virtual void PreShadow(int sublayer, ShaderBase& shader) {}

			virtual ShaderBase& GetDefaultShader();
			virtual ShaderBase& GetDefaultShadowShader();

			std::reference_wrapper<Transform> transform;
			std::shared_ptr<MeshGeometry> mesh;
			std::shared_ptr<MeshMaterial> material;
			Vector4 colour;
			Vector2 texScale = Vector2(1.0f);
		};
	}
}
