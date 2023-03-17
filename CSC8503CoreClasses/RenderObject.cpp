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
#include "ShaderBase.h"
#include "TextureBase.h"
#include "Transform.h"

using namespace NCL;
using namespace CSC8503;
using namespace Rendering;

RenderObject::RenderObject(Transform& parentTransform, std::shared_ptr<MeshGeometry> mesh, std::shared_ptr<MeshMaterial> material) :
transform(parentTransform), mesh(mesh), material(material), colour(1.0f, 1.0f, 1.0f, 1.0f) {
}

RenderObject::RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader): 
	//transform(parentTransform), mesh(mesh),texture(tex),shader(shader), colour(1.0f, 1.0f, 1.0f, 1.0f) {
{
}

RenderObject::RenderObject(RenderObject& other, Transform& parentTransform) :
transform(parentTransform), mesh(other.mesh), material(other.material), colour(other.colour) {
}

RenderObject::~RenderObject() {
}

void RenderObject::DrawToGBuffer() {
	if (!mesh) {
		return;
	}

	for (unsigned int l = 0; l < mesh->GetSubMeshCount(); l++) {
		PreDraw(l);
		mesh->Draw(l);
	}
}

void RenderObject::DrawToShadowMap() {
	if (!mesh) {
		return;
	}

	for (unsigned int l = 0; l < mesh->GetSubMeshCount(); l++) {
		PreShadow(l);
		mesh->Draw(l);
	}
}

void RenderObject::PreDraw(int sublayer) {
	MeshMaterial& mat = material ? *material : *AssetLibrary<MeshMaterial>::GetAsset("default");

	const MeshMaterialEntry* entry = mat.GetMaterialForLayer(sublayer);
	entry = entry ? entry : mat.GetMaterialForLayer(0);

	ShaderBase* shader = entry->GetShader();
	shader = shader ? shader : &GetDefaultShader();

	shader->Bind();

	Matrix4 modelMatrix = transform.get().GetGlobalMatrix();
	shader->SetUniformMatrix("modelMatrix", modelMatrix);

	shader->SetUniformFloat("modelColour", colour);
	shader->SetUniformFloat("texScale", texScale);

	TextureBase* diffuse = entry->GetTexture("Albedo");
	diffuse = diffuse ? diffuse : AssetLibrary<TextureBase>::GetAsset("defaultAlbedo").get();
	diffuse->Bind(0);

	TextureBase* bump = entry->GetTexture("Bump");
	bump = bump ? bump : AssetLibrary<TextureBase>::GetAsset("defaultBump").get();
	bump->Bind(1);

	TextureBase* spec = entry->GetTexture("Spec");
	spec = spec ? spec : AssetLibrary<TextureBase>::GetAsset("defaultSpec").get();
	spec->Bind(2);

	PreDraw(sublayer, *shader);
}

void RenderObject::PreShadow(int sublayer) {
	MeshMaterial& mat = material ? *material : *AssetLibrary<MeshMaterial>::GetAsset("default");

	const MeshMaterialEntry* entry = mat.GetMaterialForLayer(sublayer);
	entry = entry ? entry : mat.GetMaterialForLayer(0);

	ShaderBase* shader = entry->GetShadowShader();
	shader = shader ? shader : &GetDefaultShadowShader();

	shader->Bind();

	Matrix4 modelMatrix = transform.get().GetGlobalMatrix();
	shader->SetUniformMatrix("modelMatrix", modelMatrix);

	PreShadow(sublayer, *shader);
}

ShaderBase& RenderObject::GetDefaultShader() {
	return *AssetLibrary<ShaderBase>::GetAsset("modelDefault");
}

ShaderBase& RenderObject::GetDefaultShadowShader() {
	return *AssetLibrary<ShaderBase>::GetAsset("shadowDefault");
}
