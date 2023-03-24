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

namespace NCL {
	namespace Rendering {
		/**
		 * @brief OpenGL implementation of mesh geometry.
		 */
		class OGLMesh : public MeshGeometry {
		public:
			friend class OGLRenderer;
			OGLMesh();
			OGLMesh(const std::string& filename);
			~OGLMesh();

			void Initilize() override;

			void Draw() override;
			void Draw(unsigned int subLayer) override;

			void RecalculateNormals();

			void UploadToGPU() override;
			void UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount);

			/**
			 * @brief Load an existing mesh from filename. 
			 */
			static std::unique_ptr<MeshGeometry> LoadMesh(const std::string& filename);
			/**
			 * @brief Create a new uninitialized mesh. 
			 */
			static std::unique_ptr<MeshGeometry> CreateMesh();
		private:
			void CreateVertexBuffer(GLuint& buffer, int byteCount, char* data);
			void BindVertexAttribute(int attribSlot, int bufferID, int bindingID, int elementCount, int elementSize, int elementOffset);

			int subCount = 1;

			GLuint vao = 0;
			GLuint indexBuffer = 0;
			GLuint attributeBuffers[VertexAttribute::MAX_ATTRIBUTES]{};
		};
	}
}
