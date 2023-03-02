/**
 * @file   PaintRenderObject.h
 * @brief  
 * 
 * @author Harry Brettell
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RenderObject.h"

#include "OGLTexture.h"
#include "ShaderBase.h"

#include <vector>

namespace NCL::CSC8503 {
	struct PaintCollision{
		PaintCollision(Vector3 center, float radius, Vector3 colour) {
			this->center = center;
			this->radius = radius;
			this->colour = colour;
		}
		PaintCollision(Vector3 center, float radius) {
			this->center = center;
			this->radius = radius;
			this->colour = Vector3(1, 1, 1);
		}

		Vector3 center;
		float radius;
		Vector3 colour;
	};

	class PaintRenderObject : public RenderObject {
	public:
		PaintRenderObject(Transform* parentTransform, MeshGeometry* mesh, MeshMaterial* material);
		PaintRenderObject(RenderObject& other, Transform* parentTransform);
		~PaintRenderObject();

		OGLTexture* GetPaintTexture() {
			return paintTexture;
		}
		std::vector<PaintCollision> GetPaintCollisions() {
			return paintCollisions;
		}
		void ClearPaintCollisions() {
			paintCollisions.clear();
		}
		void AddPaintCollision(PaintCollision collision) {
			paintCollisions.push_back(collision);
		}

		int GetWidth() const {
			return width;
		}
		int GetHeight() const {
			return height;
		}
	protected:
		void PreDraw(int sublayer, ShaderBase* shader) override;

		ShaderBase* GetDefaultShader() override;
	private:
		OGLTexture* paintTexture;
		std::vector<PaintCollision> paintCollisions;

		int width;
		int height;
	};
}