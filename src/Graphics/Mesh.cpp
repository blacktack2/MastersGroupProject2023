/**
 * @file   Mesh.cpp
 * @brief  See Mesh.h.
 * 
 * @author Stuart Lewis
 * @date   January 2023
 */
#include "Mesh.h"

using namespace paintHell;
using namespace core;
using namespace graphics;

Mesh::Mesh() {
	glGenVertexArrays(1, &mArrayObject);
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &mArrayObject);

	delete[] mPositions;
	delete[] mIndices;
}

void Mesh::draw() const {
	glBindVertexArray(mArrayObject);
	if (mVBOs[(size_t)VBOIndex::INDICES])
		glDrawElements(mType, mNumIndices, GL_UNSIGNED_INT, nullptr);
	else
		glDrawArrays(mType, 0, mNumVertices);
	glBindVertexArray(0);
}

void Mesh::drawInstanced(GLsizei count) const {
	glBindVertexArray(mArrayObject);
	if (mVBOs[(size_t)VBOIndex::INDICES])
		glDrawElementsInstanced(mType, mNumIndices, GL_UNSIGNED_INT, nullptr, count);
	else
		glDrawArraysInstanced(mType, 0, mNumVertices, count);
	glBindVertexArray(0);
}

std::shared_ptr<Mesh> Mesh::generateQuad() {
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	mesh->mType = GL_TRIANGLE_STRIP;
	mesh->mNumVertices = 4;

	mesh->mPositions = new glm::vec3[]{
		{-1.0f, -1.0f, 0.0f},
		{ 1.0f, -1.0f, 0.0f},
		{-1.0f,  1.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f},
	};

	mesh->bufferData();
	return mesh;
}

void Mesh::bufferData() {
	glBindVertexArray(mArrayObject);

	if (mPositions)
		uploadAttribute(VBOIndex::POSITION, mNumVertices, sizeof(glm::vec3), 3, mPositions, "Positions");

	if (mIndices) {
		GLuint& id = mVBOs[(size_t)VBOIndex::INDICES];
		glGenBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices, mIndices, GL_STATIC_DRAW);

		glObjectLabel(GL_BUFFER, id, -1, "Indices");
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::uploadAttribute(VBOIndex index, GLuint numElements, GLsizei dataTypeSize, GLint attribSize,
	const void* data, const char* debugName) {
	GLuint& id = mVBOs[(size_t)index];
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);

	glBufferData(GL_ARRAY_BUFFER, numElements * dataTypeSize, data, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)index, attribSize, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray((GLuint)index);

	glObjectLabel(GL_BUFFER, id, -1, debugName);
}
