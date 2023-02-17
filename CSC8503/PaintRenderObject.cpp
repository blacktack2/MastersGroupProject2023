#include "PaintRenderObject.h"
#include "AssetLibrary.h"
#include "OGLShader.h"

using namespace NCL::CSC8503;

PaintRenderObject::PaintRenderObject(Transform* parentTransform, MeshGeometry* mesh, TextureBase* tex) : RenderObject(parentTransform, mesh, tex, AssetLibrary::GetShader("paint")) {
	width = 1024;
	height = 1024;
	paintTexture = new OGLTexture(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	paintTexture->Bind();
	paintTexture->SetFilters(GL_LINEAR, GL_LINEAR);
	paintTexture->Unbind();
}

PaintRenderObject::PaintRenderObject(RenderObject& other, Transform* parentTransform) : RenderObject(other, parentTransform) {
	width = 1024;
	height = 1024;
	paintTexture = new OGLTexture(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	paintTexture->Bind();
	paintTexture->SetFilters(GL_LINEAR, GL_LINEAR);
	paintTexture->Unbind();
}

PaintRenderObject::~PaintRenderObject() {
	delete paintTexture;
}

void PaintRenderObject::ConfigerShaderExtras(OGLShader* shaderOGL) const {
	glUniform1i(glGetUniformLocation(shaderOGL->GetProgramID(), "paintTex"), 2);
	paintTexture->Bind(2);
}
