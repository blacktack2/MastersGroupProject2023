/**
 * @file   RenderObject.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "TextureBase.h"
#include "ShaderBase.h"

#include <functional>
#include <optional>

namespace NCL {
	using namespace NCL::Rendering;

	class MeshMaterial;
	class MeshGeometry;
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

			void SetTransform(Transform& transform){
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
			
			void Draw();
		protected:
			virtual void PreDraw(int sublayer);
			virtual void PreDraw(int sublayer, ShaderBase& shader) {}

			virtual ShaderBase& GetDefaultShader();

			std::reference_wrapper<Transform> transform;
			std::shared_ptr<MeshGeometry> mesh;
			std::shared_ptr<MeshMaterial> material;
			Vector4 colour;
		};
	}
}
