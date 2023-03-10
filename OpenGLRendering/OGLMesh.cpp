/**
 * @file   OGLMesh.cpp
 * @brief  See OGLMesh.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "OGLMesh.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

using namespace NCL;
using namespace Rendering;
using namespace Maths;

OGLMesh::OGLMesh() {
	vao = 0;
	subCount = 1;

	for (int i = 0; i < VertexAttribute::MAX_ATTRIBUTES; ++i) {
		attributeBuffers[i] = 0;
	}
	indexBuffer = 0;
}

OGLMesh::OGLMesh(const std::string& filename) : MeshGeometry(filename){
	vao = 0;
	subCount = 1;

	for (int i = 0; i < VertexAttribute::MAX_ATTRIBUTES; ++i) {
		attributeBuffers[i] = 0;
	}
	indexBuffer = 0;
}

OGLMesh::~OGLMesh() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(VertexAttribute::MAX_ATTRIBUTES, attributeBuffers);
	glDeleteBuffers(1, &indexBuffer);
}

void OGLMesh::Draw() {
	Draw(0);
	for (unsigned int layer = 1; layer < GetSubMeshCount(); layer++) {
		Draw(layer);
	}
}

void OGLMesh::Draw(unsigned int subLayer) {
	glBindVertexArray(GetVAO());

	GLuint mode = 0;
	GLsizei count = 0;
	int offset = 0;

	if (GetSubMeshCount() == 0) {
		if (GetIndexCount() > 0) {
			count = GetIndexCount();
		} else {
			count = GetVertexCount();
		}
	} else {
		const std::optional<SubMesh> m = GetSubMesh(subLayer);
		if (!m) {
			return;
		}
		offset = m.value().start;
		count = m.value().count;
	}

	switch (GetPrimitiveType()) {
		case GeometryPrimitive::Triangles     : mode = GL_TRIANGLES     ; break;
		case GeometryPrimitive::Points        : mode = GL_POINTS        ; break;
		case GeometryPrimitive::Lines         : mode = GL_LINES         ; break;
		case GeometryPrimitive::TriangleFan   : mode = GL_TRIANGLE_FAN  ; break;
		case GeometryPrimitive::TriangleStrip : mode = GL_TRIANGLE_STRIP; break;
		case GeometryPrimitive::Patches       : mode = GL_PATCHES       ; break;
	}

	if (GetIndexCount() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glDrawElements(mode, count, GL_UNSIGNED_INT, (const GLvoid*)(offset * sizeof(unsigned int)));
	} else {
		glDrawArrays(mode, 0, count);
	}

	glBindVertexArray(0);
}

void CreateVertexBuffer(GLuint& buffer, int byteCount, char* data) {
	if (!buffer) {
		glGenBuffers(1, &buffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, byteCount, data, GL_STATIC_DRAW);
}

void OGLMesh::BindVertexAttribute(int attribSlot, int buffer, int bindingID, int elementCount, int elementSize, int elementOffset) {
	glEnableVertexAttribArray(attribSlot);
	glVertexAttribFormat(attribSlot, elementCount, GL_FLOAT, false, 0);
	glVertexAttribBinding(attribSlot, bindingID);

	glBindVertexBuffer(bindingID, buffer, elementOffset, elementSize);
}

void OGLMesh::UploadToGPU() {
	if (!ValidateMeshData()) {
		return;
	}
	if (!vao) {
		glGenVertexArrays(1, &vao);
	}
	glBindVertexArray(vao);

	int numVertices = GetVertexCount();
	int numIndices  = GetIndexCount();

	if (!GetPositionData().empty()) {
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Positions], numVertices * sizeof(Vector3), (char*)GetPositionData().data());
		BindVertexAttribute(VertexAttribute::Positions, attributeBuffers[VertexAttribute::Positions], VertexAttribute::Positions, 3, sizeof(Vector3), 0);
	}

	if (!GetColourData().empty()) { //buffer colour data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Colours], numVertices * sizeof(Vector4), (char*)GetColourData().data());
		BindVertexAttribute(VertexAttribute::Colours, attributeBuffers[VertexAttribute::Colours], VertexAttribute::Colours, 4, sizeof(Vector4), 0);
	}
	if (!GetTextureCoordData().empty()) { //Buffer texture data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::TextureCoords], numVertices * sizeof(Vector2), (char*)GetTextureCoordData().data());
		BindVertexAttribute(VertexAttribute::TextureCoords, attributeBuffers[VertexAttribute::TextureCoords], VertexAttribute::TextureCoords, 2, sizeof(Vector2), 0);
	}

	if (!GetNormalData().empty()) { //Buffer normal data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Normals], numVertices * sizeof(Vector3), (char*)GetNormalData().data());
		BindVertexAttribute(VertexAttribute::Normals, attributeBuffers[VertexAttribute::Normals], VertexAttribute::Normals, 3, sizeof(Vector3), 0);
	}

	if (!GetTangentData().empty()) { //Buffer tangent data
		CreateVertexBuffer(attributeBuffers[VertexAttribute::Tangents], numVertices * sizeof(Vector4), (char*)GetTangentData().data());
		BindVertexAttribute(VertexAttribute::Tangents, attributeBuffers[VertexAttribute::Tangents], VertexAttribute::Tangents, 4, sizeof(Vector4), 0);
	}

	if (!GetSkinWeightData().empty()) { //Skeleton weights
		CreateVertexBuffer(attributeBuffers[VertexAttribute::JointWeights], numVertices * sizeof(Vector4), (char*)GetSkinWeightData().data());
		BindVertexAttribute(VertexAttribute::JointWeights, attributeBuffers[VertexAttribute::JointWeights], VertexAttribute::JointWeights, 4, sizeof(Vector4), 0);
	}

	if (!GetSkinIndexData().empty()) { //Skeleton joint indices
		CreateVertexBuffer(attributeBuffers[VertexAttribute::JointIndices], numVertices * sizeof(Vector4), (char*)GetSkinIndexData().data());
		BindVertexAttribute(VertexAttribute::JointIndices, attributeBuffers[VertexAttribute::JointIndices], VertexAttribute::JointIndices, 4, sizeof(Vector4), 0);
	}

	if (!GetIndexData().empty()) { //buffer index data
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), (int*)GetIndexData().data(), GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

void OGLMesh::UpdateGPUBuffers(unsigned int startVertex, unsigned int vertexCount) {
	if (!GetPositionData().empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Positions]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector3), vertexCount * sizeof(Vector3), (char*)&GetPositionData()[startVertex]);
	}

	if (!GetColourData().empty()) { //buffer colour data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Colours]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4), (char*)&GetColourData()[startVertex]);
	}
	if (!GetTextureCoordData().empty()) { //Buffer texture data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::TextureCoords]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector2), vertexCount * sizeof(Vector2), (char*)&GetTextureCoordData()[startVertex]);
	}

	if (!GetNormalData().empty()) { //Buffer normal data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Normals]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector3), vertexCount * sizeof(Vector3), (char*)&GetNormalData()[startVertex]);
	}

	if (!GetTangentData().empty()) { //Buffer tangent data
		glBindBuffer(GL_ARRAY_BUFFER, attributeBuffers[VertexAttribute::Tangents]);
		glBufferSubData(GL_ARRAY_BUFFER, startVertex * sizeof(Vector4), vertexCount * sizeof(Vector4), (char*)&GetTangentData()[startVertex]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::unique_ptr<MeshGeometry> OGLMesh::LoadMesh(const std::string& filename) {
	std::unique_ptr<MeshGeometry> mesh = std::make_unique<OGLMesh>(filename);
	return mesh;
}

std::unique_ptr<MeshGeometry> OGLMesh::CreateMesh() {
	return std::make_unique<OGLMesh>();
}
std::unique_ptr<MeshGeometry> OGLMesh::CreateMeshAndInit() {
	std::unique_ptr<MeshGeometry> mesh = std::make_unique<OGLMesh>();
	mesh->UploadToGPU();
	return mesh;
}


void OGLMesh::RecalculateNormals() {
	normals.clear();

	if (indices.size() > 0) {
		for (size_t i = 0; i < positions.size(); i++) {
			normals.emplace_back(Vector3());
		}

		for (size_t i = 0; i < indices.size(); i += 3) {
			Vector3& a = positions[indices[i+0]];
			Vector3& b = positions[indices[i+1]];
			Vector3& c = positions[indices[i+2]];

			Vector3 normal = Vector3::Cross(b - a, c - a);
			normal.Normalise();

			normals[indices[i + 0]] += normal;
			normals[indices[i + 1]] += normal;
			normals[indices[i + 2]] += normal;
		}
		for (size_t i = 0; i < normals.size(); ++i) {
			normals[i].Normalise();
		}
	}
	else {

	}
}