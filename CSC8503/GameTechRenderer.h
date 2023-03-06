/**
 * @file   GameTechRenderer.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @author Yifei Hu
 * @date   February 2023
 */
#pragma once
#include "OGLRenderer.h"

#include "SkyboxRPass.h"
#include "ModelRPass.h"
#include "LightingRPass.h"
#include "SSAORPass.h"
#include "CombineRPass.h"
#include "BloomRPass.h"
#include "HDRRPass.h"
#include "PresentRPass.h"
#include "DebugRPass.h"
#include "MenuRPass.h"
#include "PaintingRPass.h"

#include "GameWorld.h"

#include <algorithm>
#include <memory>
#include <string>

namespace NCL {
	class MeshAnimation;
	class MeshGeometry;
	class MeshMaterial;
}

namespace NCL::Rendering {
	class ShaderBase;
	class TextureBase;
}
namespace NCL::CSC8503 {
	class RenderObject;

	class GameTechRenderer : public OGLRenderer {

	public:
		static GameTechRenderer& instance() {
			static GameTechRenderer INSTANCE;
			return INSTANCE;
		}

		void InitPipeline();

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
		inline MenuRPass& GetMenuRPass() {
			return *menuPass;
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

		void SetSSAORadius(float radius) {
			ssaoRadius = radius;
			ssaoPass->SetRadius(ssaoRadius);
		}
		inline float GetSSAORadius() {
			return ssaoRadius;
		}

		void SetSSAOBias(float bias) {
			ssaoBias = bias;
			ssaoPass->SetBias(ssaoBias);
		}
		inline float GetSSAOBias() {
			return ssaoBias;
		}
	protected:
		GameTechRenderer();
		~GameTechRenderer();

		GameWorld& gameWorld;

		void BuildObjectList();
		void SortObjectList();

		std::vector<const RenderObject*> activeObjects;

		std::unique_ptr<SkyboxRPass>   skyboxPass;
		std::unique_ptr<ModelRPass>    modelPass;
		std::unique_ptr<LightingRPass> lightingPass;
		std::unique_ptr<SSAORPass>     ssaoPass;
		std::unique_ptr<CombineRPass>  combinePass;
		std::unique_ptr<BloomRPass>    bloomPass;
		std::unique_ptr<HDRRPass>      hdrPass;
		std::unique_ptr<PresentRPass>  presentPass;
		std::unique_ptr<DebugRPass>    debugPass;
		std::unique_ptr<MenuRPass>     menuPass;
		std::unique_ptr<PaintingRPass> paintingRPass;

		float gamma = 2.2f;

		size_t bloomAmount = 5;
		float bloomBias = 0.04f;

		float hdrExposure = 1.0f;

		float ssaoRadius = 0.5f;
		float ssaoBias = 0.025f;
	};
}

