/**
 * @file   OGLMesh.h
 * @brief  OpenGL implementation of mesh geometry.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "MeshGeometry.h"
#include "glad\gl.h"

#include <string>

namespace NCL::Rendering {
	/**
	 * @brief OpenGL implementation of mesh geometry.
	 */
	class OGLMesh : public NCL::MeshGeometry {
	public:
		friend class OGLRenderer;
		OGLMesh();
		OGLMesh(const std::string& filename);
		~OGLMesh();

		virtual void Draw() override;
		virtual void Draw(unsigned int subLayer) override;

		void RecalculateNormals();

		void UploadToGPU(Rendering::RendererBase* renderer = nullptr) override;
		void UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount);

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