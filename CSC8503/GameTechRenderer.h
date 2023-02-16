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
#include "CombineRPass.h"
#include "BloomRPass.h"
#include "HDRRPass.h"
#include "PresentRPass.h"
#include "DebugRPass.h"

#include "GameWorld.h"

#include <algorithm>
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
		inline CombineRPass& GetCombinePass() {
			return *combinePass;
		}
		inline BloomRPass& GetBloomPass() {
			return *bloomPass;
		}
		inline HDRRPass& GetHDRPass() {
			return *hdrPass;
		}
		inline PresentRPass& GetPresentPass() {
			return *presentPass;
		}
		inline DebugRPass& GetDebugRPass() {
			return *debugPass;
		}

		virtual void Update(float dt) override;

		void SetGamma(float g) {
			gamma = g;
			modelPass->SetGamma(gamma);
			presentPass->SetGamma(gamma);
		}
		inline float GetGamma() {
			return gamma;
		}

		void SetBloomAmount(size_t depth) {
			bloomAmount = std::min(std::max(depth, 1ull), 100ull);
			bloomPass->SetBloomDepth(bloomAmount);
		}
		inline size_t GetBloomAmount() {
			return bloomAmount;
		}

		void SetBloomBias(float bias) {
			bloomBias = bias;
			bloomPass->SetBias(bloomBias);
		}
		inline float GetBloomBias() {
			return bloomBias;
		}

		void SetHDRExposure(float exposure) {
			hdrExposure = exposure;
			hdrPass->SetExposure(hdrExposure);
		}
		inline float GetHDRExposure() {
			return hdrExposure;
		}
	protected:
		GameWorld& gameWorld;

		void BuildObjectList();
		void SortObjectList();

		vector<const RenderObject*> activeObjects;

		SkyboxRPass* skyboxPass;
		ModelRPass* modelPass;
		LightingRPass* lightingPass;
		CombineRPass* combinePass;
		BloomRPass* bloomPass;
		HDRRPass* hdrPass;
		PresentRPass* presentPass;
		DebugRPass* debugPass;

		float gamma = 2.2f;
		size_t bloomAmount = 5;
		float bloomBias = 0.04f;
		float hdrExposure = 1.0f;
	};
}

