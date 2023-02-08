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
#include "ModelRPass.h"
#include "LightingRPass.h"
#include "GBufferRPass.h"
#include "DebugRPass.h"
#include "PresentRPass.h"

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

		inline SkyboxRPass& GetSkyboxPass() {
			return *skyboxPass;
		}
		inline ModelRPass& GetModelPass() {
			return *modelPass;
		}
		inline LightingRPass& GetLightingPass() {
			return *lightingPass;
		}
		inline GBufferRPass& GetGBufferPass() {
			return *gbufferPass;
		}
		inline PresentRPass& GetPresentPass() {
			return *presentPass;
		}
		inline DebugRPass& GetDebugRPass() {
			return *debugPass;
		}

		virtual void Update(float dt) override;
	protected:

		void RenderFrame() override;

		GameWorld& gameWorld;

		void BuildObjectList();
		void SortObjectList();

		vector<const RenderObject*> activeObjects;

		SkyboxRPass* skyboxPass;
		ModelRPass* modelPass;
		LightingRPass* lightingPass;
		GBufferRPass* gbufferPass;
		PresentRPass* presentPass;
		DebugRPass* debugPass;
	};
}

