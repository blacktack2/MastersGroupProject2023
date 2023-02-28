#include "RenderObject.h"
#include "MeshGeometry.h"

using namespace NCL;
using namespace CSC8503;
RenderObject::RenderObject(Transform* parentTransform, CSC8503::MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader) {
	this->transform = parentTransform;
	this->mesh = mesh;
	this->texture = tex;
	this->shader = shader;
	this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

RenderObject::RenderObject(RenderObject& other, Transform* parentTransform) {
	transform = parentTransform;
	mesh = other.mesh;
	texture = other.texture;
	shader = other.shader;
	colour = other.colour;
}

RenderObject::~RenderObject() {
}
