#include "AnimatedRenderObject.h"
#include "AssetLibrary.h"
#include "Assets.h"
#include "OGLShader.h"

using namespace NCL::CSC8503;

AnimatedRenderObject::AnimatedRenderObject(MeshMaterial* mater, MeshAnimation* ani, MeshGeometry* msh, std::vector<OGLTexture*>  matTex, Transform* parentTransform) :
RenderObject(parentTransform, AssetLibrary::GetShader("animation")) {
	material = mater;
	animMesh = msh;
	anim = ani;
	matTextures = matTex;

	currentFrame = 0;
	frameTime = 0.0f;
}

AnimatedRenderObject::~AnimatedRenderObject() {
	
}

void AnimatedRenderObject::ConfigerShaderExtras(OGLShader* shaderOGL) const {
	
}
