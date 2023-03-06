/**
 * @file   RenderObject.cpp
 * @brief  See RenderObject.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "RenderObject.h"

#include "AssetLibrary.h"
#include "MeshGeometry.h"
#include "MeshMaterial.h"
#include "Transform.h"

using namespace NCL;
using namespace CSC8503;

RenderObject::RenderObject(Transform& parentTransform, std::shared_ptr<MeshGeometry> mesh, std::shared_ptr<MeshMaterial> material) :
transform(parentTransform), mesh(mesh), material(material), colour(1.0f, 1.0f, 1.0f, 1.0f) {
}

RenderObject::RenderObject(RenderObject& other, Transform& parentTransform) :
transform(parentTransform), mesh(other.mesh), material(other.material), colour(other.colour) {
}

RenderObject::~RenderObject() {
}

void RenderObject::Draw() {
	if (!mesh) {
		return;
	}

	for (unsigned int l = 0; l < mesh->GetSubMeshCount(); l++) {
		PreDraw(l);
		mesh->Draw(l);
	}
}

void RenderObject::PreDraw(int sublayer) {
	MeshMaterial& mat = material ? *material : *AssetLibrary::instance().GetMaterial("default");

	auto sublayerEntry = mat.GetMaterialForLayer(sublayer);
	MeshMaterialEntry& entry = sublayerEntry ? sublayerEntry.value() : mat.GetMaterialForLayer(0).value();

	auto entryShader = entry.GetShader();
	ShaderBase& shader = entryShader ? entryShader.value().get() : GetDefaultShader();

	shader.Bind();

	Matrix4 modelMatrix = transform.get().GetGlobalMatrix();
	shader.SetUniformMatrix("modelMatrix", modelMatrix);

	shader.SetUniformFloat("modelColour", colour);

	auto entryDiffuse = entry.GetTexture("Diffuse");
	TextureBase& diffuse = entryDiffuse ? entryDiffuse.value().get() : *AssetLibrary::instance().GetTexture("defaultDiffuse");
	diffuse.Bind(0);

	auto entryBump = entry.GetTexture("Bump");
	TextureBase& bump = entryBump ? entryBump.value().get() : *AssetLibrary::instance().GetTexture("defaultBump");
	bump.Bind(1);

	auto entrySpec = entry.GetTexture("Spec");
	TextureBase& spec = entrySpec ? entrySpec.value().get() : *AssetLibrary::instance().GetTexture("defaultSpec");
	spec.Bind(2);

	PreDraw(sublayer, shader);
}

ShaderBase& RenderObject::GetDefaultShader() {
	return *AssetLibrary::instance().GetShader("modelDefault");
}
