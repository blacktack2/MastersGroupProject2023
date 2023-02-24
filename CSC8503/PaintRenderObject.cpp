#include "PaintRenderObject.h"
#include "AssetLibrary.h"
#include "OGLShader.h"

using namespace NCL::CSC8503;

PaintRenderObject::PaintRenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex) : RenderObject(parentTransform, mesh, tex, AssetLibrary::GetShader("paint")) {
	paintTexture = new OGLTexture(1000, 1000, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	paintTexture->Bind();
	paintTexture->SetFilters(GL_LINEAR, GL_LINEAR);
	paintTexture->Unbind();
	paintCollisions.push_back(PaintCollision(Vector3(0, 0, 0), 1));
	width = 1000;
	height = 1000;
}

PaintRenderObject::PaintRenderObject(RenderObject& other, Transform* parentTransform) : RenderObject(other, parentTransform) {
	paintTexture = new OGLTexture(1000, 1000, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	width = 1000;
	height = 1000;
}

PaintRenderObject::~PaintRenderObject() {
	delete paintTexture;
}

void PaintRenderObject::ConfigerShaderExtras(OGLShader* shaderOGL) const {
	paintTexture->Bind(2, shaderOGL->GetUniformLocation("paintTex"));
}
