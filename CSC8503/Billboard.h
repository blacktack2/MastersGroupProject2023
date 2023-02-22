#pragma once
#include "GameTechRenderer.h"

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class Billboard {

	public:
		Billboard(Vector3 position);
		void update(Vector3 playerPosition);

	private:
		Vector3 position;
		Matrix4 modelTransform;

		OGLMesh* quad;

		OGLShader* billboardShader;
		OGLTexture* billboardTexture;
	};
}
