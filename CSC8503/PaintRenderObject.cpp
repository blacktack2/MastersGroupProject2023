#include "PaintRenderObject.h"
#include "AssetLibrary.h"
#include "OGLShader.h"

using namespace NCL::CSC8503;

PaintRenderObject::PaintRenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex) : RenderObject(parentTransform, mesh, tex, AssetLibrary::GetShader("paint")) {
	paintTexture = new OGLTexture(1000, 1000, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	worldTexture = new OGLTexture(1000, 1000, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	paintCollisions.push_back(PaintCollision(Vector3(0, 0, 0), 1));
}

PaintRenderObject::PaintRenderObject(RenderObject& other, Transform* parentTransform) : RenderObject(other, parentTransform) {
	paintTexture = new OGLTexture(1000, 1000, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
}

PaintRenderObject::~PaintRenderObject() {
	delete paintTexture;
}

void PaintRenderObject::ConfigerShaderExtras(OGLShader* shaderOGL) const {
	glUniform1i(glGetUniformLocation(shaderOGL->GetProgramID(), "paintTex"), 2);
	paintTexture->Bind(2);
}
