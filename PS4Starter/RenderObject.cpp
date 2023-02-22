#include "RenderObject.h"
#include "../Common/ShaderBase.h"
#include "../Common/TextureBase.h"

using namespace NCL;
using namespace NCL::Rendering;
using namespace NCL::PS4;

RenderObject::RenderObject()
{
	mesh	= nullptr;
	shader	= nullptr;

	for (int i = 0; i < TEXTURE_COUNT; ++i) {
		textures[i] = nullptr;
	}
}

RenderObject::~RenderObject()
{

}

RenderObject::RenderObject(MeshGeometry* m, NCL::Rendering::ShaderBase* s, NCL::Rendering::TextureBase* t) {
	mesh		= m;
	shader		= s;
	textures[0] = t;
}

const Maths::Matrix4& RenderObject::GetLocalTransform() const {
	return localTransform;
}

void	RenderObject::SetLocalTransform(const Maths::Matrix4& mat) {
	localTransform = mat;
}