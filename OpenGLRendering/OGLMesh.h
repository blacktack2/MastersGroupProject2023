/**
 * @file   OGLMesh.h
 * @brief  OpenGL implementation of mesh geometry.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "MeshGeometry.h"
#include "glad\gl.h"

#include <memory>
#include <string>

namespace NCL::Rendering {
	/**
	 * @brief OpenGL implementation of mesh geometry.
	 */
	class OGLMesh : public MeshGeometry {
	public:
		friend class OGLRenderer;
		OGLMesh();
		OGLMesh(const std::string& filename);
		~OGLMesh();

		virtual void Draw() override;
		virtual void Draw(unsigned int subLayer) override;

		void RecalculateNormals();

		void UploadToGPU() override;
		void UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount);

		static std::unique_ptr<MeshGeometry> LoadMesh(const std::string& filename);
		static std::unique_ptr<MeshGeometry> CreateMesh();
	protected:
		GLuint GetVAO() const { return vao; }
		void BindVertexAttribute(int attribSlot, int bufferID, int bindingID, int elementCount, int elementSize, int elementOffset);

		int subCount;

		GLuint vao;
		GLuint oglType;
		GLuint attributeBuffers[VertexAttribute::MAX_ATTRIBUTES];
		GLuint indexBuffer;
	};
}