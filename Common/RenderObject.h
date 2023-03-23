#pragma once
#include "TextureBase.h"
#include "ShaderBase.h"
#include "Vector4.h"
#include "Vector3.h"
#include <gnm.h>


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
			struct PaintCollision {
				PaintCollision(const Vector3& cen, float r, const Vector3& col) {
					center = cen;
					radius = r;
					colour = col;
				}

				Vector3 center;
				float radius;
				Vector3 colour;
			};

			RenderObject(Transform* parentTransform, CSC8503::MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader = nullptr);
			RenderObject(RenderObject& other, Transform* parentTransform);
			~RenderObject();

			void SetDefaultTexture(TextureBase* t) {
				albedoTexture = t;
			}

			TextureBase* GetDefaultTexture() const {
				return albedoTexture;
			}
			
			/*void SetPaintTexture(TextureBase* tex, const sce::Gnm::RenderTarget& target) {
				paintTexture = tex;
				paintTarget  = target;
			}

			TextureBase* GetPaintTexture() const {
				return paintTexture;
			}
			const sce::Gnm::RenderTarget& GetPaintTarget() const {
				return paintTarget;
			}*/

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


			inline const std::vector<PaintCollision>& GetPaintCollisions() {
				return paintCollisions;
			}
			inline void AddPaintCollision(const Vector3& center, float radius, const Vector3& colour = Vector3(1.0f)) {
				paintCollisions.emplace_back(center, radius, colour);
			}
			inline void ClearPaintCollisions() {
				paintCollisions.clear();
			}

		protected:
			Transform* transform;
			CSC8503::MeshGeometry* mesh;

			TextureBase* albedoTexture;
			//TextureBase* paintTexture= nullptr;

			//sce::Gnm::RenderTarget paintTarget;

			ShaderBase* shader;
			std::vector<PaintCollision>  paintCollisions{};
			Vector4			colour;
		};
	}
}
