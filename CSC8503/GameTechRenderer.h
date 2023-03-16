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
#include "HudRPass.h"
#include "Hud.h"
#include "GameWorld.h"
//#include "Debug.h"
#include "DebugRPass.h"



#include <algorithm>
#include <memory>
#include <string>

namespace NCL {
	class MeshAnimation;
	class MeshGeometry;
	class MeshMaterial;
}

namespace NCL {
	namespace Rendering {
		class ShaderBase;
		class TextureBase;
	}
}
namespace NCL {
	namespace CSC8503 {
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
			inline HudRPass& GetHudRPass() {
				return *hudPass;
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

		void SetGameWorldMainCamera(int cameraNum) override {
			gameWorld.SetMainCamera(cameraNum);

		}

		int GetGameWorldMainCamera() override {
			return gameWorld.GetMainCameraIndex();
		}

		void SetBossHP(int hp) {
			bossHP = hp;
		}

		void SetPlayerHp(int id, int hp) {
			playersHP[id] = hp;
		}

		void UpdateHudDisplay(int playerID) override {
		}

		void DisplayWinLoseInformation(int playerID) override {
			gameWorld.GetCamera(playerID);
			if (bossHP <= 0) {
				debugPass->RenderWinLoseInformation(true);
			}
			else if (playersHP[playerID] <= 0) {
				debugPass->RenderWinLoseInformation(false);
			}
		}

	protected:
		GameTechRenderer();
		~GameTechRenderer();

		GameWorld& gameWorld;
		std::unique_ptr<Hud> hud = std::make_unique<Hud>();

			void SortObjectList();

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
			std::unique_ptr<HudRPass> hudPass;

			float gamma = 2.2f;

			size_t bloomAmount = 5;
			float bloomBias = 0.04f;

			float hdrExposure = 1.0f;
			int bossHP = -1;
			int playersHP[4];
			float ssaoRadius = 0.5f;
			float ssaoBias = 0.025f;
		};
	}
}

