#pragma once
#include "RenderObject.h"
#include "OGLTexture.h"
#include <vector>

namespace NCL::CSC8503 {

	struct PaintCollision{
		Vector3 center;
		float radius;
		Vector3 colour;
		PaintCollision(Vector3 center, float radius, Vector3 colour) {
			this->center = center;
			this->radius = radius;
			this->colour = colour;
		}
		PaintCollision(Vector3 center, float radius) {
			this->center = center;
			this->radius = radius;
			this->colour = Vector3(1,1,1);
		}
	};

	class PaintRenderObject : public RenderObject {
	public:
		PaintRenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex);
		PaintRenderObject(RenderObject& other, Transform* parentTransform);
		~PaintRenderObject();

		OGLTexture* GetPaintTexture() { return paintTexture; }
		std::vector<PaintCollision> GetPaintCollisions() { return paintCollisions; }
		void ClearPaintCollisions() { paintCollisions.clear(); }
		void AddPaintCollision(PaintCollision collision) { paintCollisions.push_back(collision); }

		void ConfigerShaderExtras(OGLShader* shaderOGL) const override;

		int GetWidth() const { return width; }
		int GetHeight() const { return height; }

	private:
		OGLTexture* paintTexture;
		std::vector<PaintCollision> paintCollisions;
		int width;
		int height;
	};
}