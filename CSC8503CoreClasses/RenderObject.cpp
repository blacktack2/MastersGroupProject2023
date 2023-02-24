#include "RenderObject.h"
#include "MeshGeometry.h"

using namespace NCL::CSC8503;
using namespace NCL;

RenderObject::RenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader) {
	this->transform	= parentTransform;
	this->mesh		= mesh;
	this->texture	= tex;
	this->shader	= shader;
	this->colour	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

RenderObject::RenderObject(RenderObject& other, Transform* parentTransform) {
	transform = parentTransform;
	mesh      = other.mesh;
	texture   = other.texture;
	shader    = other.shader;
	colour    = other.colour;
}

RenderObject::RenderObject(Transform* parentTransform, ShaderBase* shader) {
	this->hasAnimation = true;
	this->transform = parentTransform;
	this->mesh = nullptr;
	this->texture = nullptr;
	this->shader = shader;
	this->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

RenderObject::~RenderObject() {
}
