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

#include "Vector2.h"
#include "Vector4.h"

namespace NCL {
	class MeshMaterial;
	class MeshGeometry;

	namespace Rendering {
		class TextureBase;
		class ShaderBase;
	}

	using namespace NCL::Maths;
	using namespace NCL::Rendering;

	namespace CSC8503 {
		class Transform;

		class RenderObject {
		public:
			struct PaintCollision {
				Vector3 center;
				float radius;
				Vector3 colour;
			};

			RenderObject(Transform& parentTransform, std::shared_ptr<MeshGeometry> mesh, std::shared_ptr<MeshMaterial> material);
			RenderObject(RenderObject& other, Transform& parentTransform);
			virtual ~RenderObject();

			std::shared_ptr<MeshGeometry> GetMesh() const {
				return mesh;
			}

			inline void SetTransform(Transform& transform) {
				this->transform = transform;
			}
			inline Transform& GetTransform() const {
				return transform;
			}

			inline void SetColour(const Vector4& c) {
				colour = c;
			}
			inline const Vector4& GetColour() const {
				return colour;
			}

			inline void SetTexScale(const Vector2& scale) {
				texScale = scale;
			}

			inline TextureBase* GetPaintTex() {
				return paintTexture.get();
			}
			void SetPaintTex(int width = 1024, int height = 1024);

			inline int GetPaintTexWidth() {
				return paintTexWidth;
			}
			inline int GetPaintTexHeight() {
				return paintTexHeight;
			}

			inline const std::vector<PaintCollision>& GetPaintCollisions() {
				return paintCollisions;
			}
			inline void AddPaintCollision(const Vector3& center, float radius, const Vector3& colour = Vector3(1.0f)) {
				paintCollisions.emplace_back(center, radius, colour);
			}
			inline void ClearPaintCollisions() {
				paintCollisions.clear();
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
		private:
			std::reference_wrapper<Transform> transform;

			std::shared_ptr<MeshGeometry> mesh;
			std::shared_ptr<MeshMaterial> material;

			std::unique_ptr<TextureBase> paintTexture;
			std::vector<PaintCollision>  paintCollisions{};

			Vector4 colour = Vector4(1.0f);
			Vector2 texScale = Vector2(1.0f);

			int paintTexWidth = 0;
			int paintTexHeight = 0;
		};
	}
}
