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

RenderObject::RenderObject(Transform* parentTransform, MeshGeometry* mesh, MeshMaterial* material) {
	this->transform = parentTransform;
	this->mesh      = mesh;
	this->material  = material;
	this->colour    = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

RenderObject::RenderObject(RenderObject& other, Transform* parentTransform) {
	transform = parentTransform;
	mesh      = other.mesh;
	material  = other.material;
	colour    = other.colour;
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
	MeshMaterial* mat = material ? material : AssetLibrary::GetMaterial("default");

	const MeshMaterialEntry* entry = mat->GetMaterialForLayer(sublayer);
	entry = entry ? entry : mat->GetMaterialForLayer(0);

	ShaderBase* shader = entry->GetShader();
	shader = shader ? shader : AssetLibrary::GetShader("defaultModel");

	shader->Bind();

	Matrix4 modelMatrix = transform->GetGlobalMatrix();
	shader->SetUniformMatrix("modelMatrix", modelMatrix);

	shader->SetUniformFloat("modelColour", colour);

	TextureBase* diffuse = entry->GetTexture("Diffuse");
	diffuse = diffuse ? diffuse : AssetLibrary::GetTexture("defaultDiffuse");
	diffuse->Bind(0);

	TextureBase* bump = entry->GetTexture("Bump");
	bump = bump ? bump : AssetLibrary::GetTexture("defaultBump");
	bump->Bind(1);

	TextureBase* spec = entry->GetTexture("Spec");
	spec = spec ? spec : AssetLibrary::GetTexture("defaultSpec");
	spec->Bind(2);

	PreDraw(sublayer, shader);
}
