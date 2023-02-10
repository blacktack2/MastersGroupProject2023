#include "PaintRenderObject.h"

using namespace NCL::CSC8503;

PaintRenderObject::PaintRenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex, ShaderBase* shader) : RenderObject(parentTransform, mesh, tex, shader) {
	paintTexture = new OGLTexture(100, 100, TexType::Colour4);
}

PaintRenderObject::PaintRenderObject(RenderObject& other, Transform* parentTransform) : RenderObject(other, parentTransform) {
	paintTexture = new OGLTexture(100, 100, TexType::Colour4);
}

PaintRenderObject::~PaintRenderObject() {
	delete paintTexture;
}
