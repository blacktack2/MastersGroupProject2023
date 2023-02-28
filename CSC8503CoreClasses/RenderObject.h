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

namespace NCL {
	using namespace NCL::Rendering;

	class MeshMaterial;
	class MeshGeometry;
	namespace CSC8503 {
		class Transform;
		using namespace Maths;

		class RenderObject {
		public:
			RenderObject(Transform* parentTransform, MeshGeometry* mesh, MeshMaterial* material);
			RenderObject(RenderObject& other, Transform* parentTransform);
			~RenderObject();

			MeshGeometry* GetMesh() const {
				return mesh;
			}


			void SetTransform(Transform* transform){
				this->transform = transform;
			}

			Transform* GetTransform() const {
				return transform;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}
			
			void Draw();
		protected:
			virtual void PreDraw(int sublayer);
			virtual void PreDraw(int sublayer, ShaderBase* shader) {}

			virtual ShaderBase* GetDefaultShader();

			Transform*    transform;
			MeshGeometry* mesh;
			MeshMaterial* material;
			Vector4       colour;
		};
	}
}
