#include "RenderObject.h"
#include "MeshGeometry.h"

using namespace NCL;
using namespace CSC8503;
RenderObject::RenderObject(Transform* parentTransform, CSC8503::MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader) {
	this->transform = parentTransform;
	this->mesh = mesh;
	this->albedoTexture = tex;
	this->shader = shader;
	this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	bumpTexture = nullptr;
}

RenderObject::RenderObject(RenderObject& other, Transform* parentTransform) {
	transform = parentTransform;
	mesh = other.mesh;
	albedoTexture = other.albedoTexture;
	shader = other.shader;
	colour = other.colour;
	bumpTexture = other.bumpTexture;
}

RenderObject::~RenderObject() {
}
