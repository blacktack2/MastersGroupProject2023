/**
 * @file   GameTechRenderer.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "OGLFrameBuffer.h"
#include "OGLMesh.h"
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLTexture.h"

#include "SkyboxRPass.h"

#include "GameWorld.h"

#include <string>

namespace NCL::Maths {
	class Vector3;
	class Vector4;
}
namespace NCL::CSC8503 {
	class RenderObject;

	class GameTechRenderer : public OGLRenderer {
	public:
		GameTechRenderer(GameWorld& world);
		~GameTechRenderer();

		MeshGeometry* LoadMesh(const std::string& name);
		TextureBase*  LoadTexture(const std::string& name);
		ShaderBase*   LoadShader(const std::string& vertex, const std::string& fragment);

		virtual void Update(float dt) override;
	protected:
		void NewRenderLines();
		void NewRenderText();

		void RenderFrame() override;

		OGLShader* defaultShader;

		GameWorld& gameWorld;

		void BuildObjectList();
		void SortObjectList();
		void RenderShadowMap();
		void RenderCamera();

		void SetDebugStringBufferSizes(size_t newVertCount);
		void SetDebugLineBufferSizes(size_t newVertCount);

		vector<const RenderObject*> activeObjects;

		SkyboxRPass* skyboxPass;

		OGLShader* debugShader;
		OGLMesh*   skyboxMesh;

		OGLShader* shadowShader;
		OGLTexture* shadowTex;
		OGLFrameBuffer* shadowFBO;
		Matrix4 shadowMatrix;

		Vector4 lightColour;
		float   lightRadius;
		Vector3 lightPosition;

		vector<Vector3> debugLineData;

		vector<Vector3> debugTextPos;
		vector<Vector4> debugTextColours;
		vector<Vector2> debugTextUVs;

		GLuint lineVAO;
		GLuint lineVertVBO;
		size_t lineCount;

		GLuint textVAO;
		GLuint textVertVBO;
		GLuint textColourVBO;
		GLuint textTexVBO;
		size_t textCount;
	};
}

