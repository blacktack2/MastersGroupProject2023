#ifdef _ORBIS
#include "PS4Mesh.h"
#include "PS4Renderer.h"
#include "Vector2.h"
#include "Vector3.h"

#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>

using namespace NCL::Maths;
using namespace NCL::Rendering;

using namespace sce;

PS4Mesh::PS4Mesh(PS4Renderer& renderer): renderer(renderer) {
	indexBuffer		= 0;
	vertexBuffer	= 0;
	attributeCount	= 0;
}

PS4Mesh::~PS4Mesh()	{
	delete[] attributeBuffers;
}

PS4Mesh::PS4Mesh(PS4Renderer& renderer, const std::string& filename) : MeshGeometry(filename), renderer(renderer) {
	indexType = sce::Gnm::IndexSize::kIndexSize32;
	primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriList;
}

PS4Mesh* PS4Mesh::GenerateQuad() {
	PS4Mesh* mesh = new PS4Mesh();

	mesh->indexType		= sce::Gnm::IndexSize::kIndexSize32;
	mesh->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriStrip;

	mesh->SetVertexPositions({ Vector3(-1.0f, -1.0f, 0.0f), Vector3(-1.0f,  1.0f, 0.0f),Vector3(1.0f, -1.0f, 0.0f), Vector3(1.0f,  1.0f, 0.0f) });

	std::vector<Vector3> normals;
	std::vector<Vector4> tangents;
	std::vector<unsigned int> indices;

	for (int i = 0; i < 4; ++i) {
		normals.emplace_back(Vector3(0, 0, 1));
		tangents.emplace_back(Vector4(1, 0, 0, 0));

		indices.emplace_back(i);
	}

	mesh->SetVertexNormals(normals);
	mesh->SetVertexTangents(tangents);
	mesh->SetVertexIndices(indices);

	return mesh;
}

PS4Mesh* PS4Mesh::GenerateSinglePoint() {
	PS4Mesh* mesh = new PS4Mesh();

	mesh->indexType		= sce::Gnm::IndexSize::kIndexSize32;
	mesh->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypePointList;

	mesh->SetVertexPositions({ Vector3(0.0f, 0.0f, 0.0f) });
	mesh->SetVertexNormals({ Vector3(0, 0, 1) });
	mesh->SetVertexTangents({ Vector4(1, 0, 0,0) });
	mesh->SetVertexIndices({ 0 });

	return mesh;
}

void NCL::Rendering::PS4Mesh::Initilize(){
	UploadToGPU();
}

void NCL::Rendering::PS4Mesh::Draw(){
	Gnmx::GnmxGfxContext& cmdList = renderer.GetCurrentGFXContext();
	cmdList.setVertexBuffers(sce::Gnm::ShaderStage::kShaderStageVs, 0, attributeCount, attributeBuffers);
	cmdList.setPrimitiveType(primitiveType);
	cmdList.setIndexSize(indexType);
	cmdList.drawIndex(GetIndexCount(), indexBuffer);

}

void NCL::Rendering::PS4Mesh::Draw(unsigned int subLayer){
	Gnmx::GnmxGfxContext& cmdList = renderer.GetCurrentGFXContext();
	cmdList.setVertexBuffers(sce::Gnm::ShaderStage::kShaderStageVs, 0, attributeCount, attributeBuffers);
	cmdList.setPrimitiveType(primitiveType);
	cmdList.setIndexSize(indexType);
	cmdList.drawIndex(GetIndexCount(), indexBuffer);
}

PS4Mesh* PS4Mesh::GenerateTriangle() {
	PS4Mesh* mesh = new PS4Mesh();

	mesh->indexType		= sce::Gnm::IndexSize::kIndexSize32;
	mesh->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriList;

	mesh->SetVertexPositions({ Vector3(0.0f, 0.5f, 0.0f), Vector3(0.5f, -0.5f, 0.0f), Vector3(-0.5f, -0.5f, 0.0f) });
	mesh->SetVertexTextureCoords({ Vector2(0.5f, 0.0f) , Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f) });
	mesh->SetVertexNormals({ Vector3(0, 0, 1),Vector3(0, 0, 1), Vector3(0, 0, 1) });
	mesh->SetVertexTangents({ Vector4(1, 0, 0, 0), Vector4(1, 0, 0,0), Vector4(1, 0, 0,0) });
	mesh->SetVertexIndices({ 0, 1, 2 });

	return mesh;
}

void PS4Mesh::UploadToGPU() {
	vertexDataSize = GetVertexCount() * sizeof(MeshVertex);
	indexDataSize  = GetIndexCount() * sizeof(int);

	indexBuffer = static_cast<int*>			(garlicAllocator->allocate(indexDataSize, Gnm::kAlignmentOfBufferInBytes));
	vertexBuffer = static_cast<MeshVertex*>	(garlicAllocator->allocate(vertexDataSize, Gnm::kAlignmentOfBufferInBytes));

	Gnm::registerResource(nullptr, ownerHandle, indexBuffer , indexDataSize , "IndexData" , Gnm::kResourceTypeIndexBufferBaseAddress, 0);
	Gnm::registerResource(nullptr, ownerHandle, vertexBuffer, vertexDataSize, "VertexData", Gnm::kResourceTypeIndexBufferBaseAddress, 0);

	for (int i = 0; i < GetVertexCount(); ++i) {
		memcpy(&vertexBuffer[i].position,	  &GetPositionData()[i], sizeof(float) * 3);
		memcpy(&vertexBuffer[i].textureCoord, &GetTextureCoordData()[i], sizeof(float) * 2);
		memcpy(&vertexBuffer[i].normal,		  &GetNormalData()[i], sizeof(float) * 3);
		memcpy(&vertexBuffer[i].tangent,	  &GetTangentData()[i], sizeof(float) * 3);
	}

	for (int i = 0; i < GetIndexCount(); ++i) { //Our index buffer might not have the same data size as the source indices?
		indexBuffer[i] = GetIndexData()[i];
	}	

	attributeCount		= 4;
	attributeBuffers	= new sce::Gnm::Buffer[4];

	InitAttributeBuffer(attributeBuffers[0], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].position));
	InitAttributeBuffer(attributeBuffers[1], Gnm::kDataFormatR32G32Float	, &(vertexBuffer[0].textureCoord));
	InitAttributeBuffer(attributeBuffers[2], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].normal));
	InitAttributeBuffer(attributeBuffers[3], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].tangent));
}

void	PS4Mesh::InitAttributeBuffer(sce::Gnm::Buffer &buffer, Gnm::DataFormat format, void*offset) {
	buffer.initAsVertexBuffer(offset, format, sizeof(MeshVertex), GetVertexCount());
	buffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
}
#endif