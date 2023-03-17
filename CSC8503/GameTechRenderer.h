/**
 * @file   GameTechRenderer.h
 * @brief  Game specific implementation of the renderer.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @author Yifei Hu
 * @date   February 2023
 */
#pragma once
#ifdef x64
#include "OGLRenderer.h"

#include "PaintingRPass.h"
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
#include "HudRPass.h"

#include "Vector3.h"

#include <algorithm>
#include <memory>
#include <string>

namespace NCL {
	namespace CSC8503 {
		class GameWorld;

		class RenderObject;

		/**
		 * @brief Game specific implementation of the renderer.
		 */
		class GameTechRenderer : public OGLRenderer {
		public:
			static GameTechRenderer& instance() {
				static GameTechRenderer INSTANCE;
				return INSTANCE;
			}

			/**
			 * @brief Initialize render passes and set the render pipeline to
			 * its default state (everything active).
			 */
			void InitPipeline();

			void SetGamma(float g);
			inline float GetGamma() {
				return gamma;
			}

			void SetSunDir(Vector3 direction);
			inline const Vector3& GetSunDir() {
				return sunDirection;
			}

			void SetBloomAmount(size_t depth);
			inline size_t GetBloomAmount() {
				return bloomAmount;
			}

			void SetBloomBias(float bias);
			inline float GetBloomBias() {
				return bloomBias;
			}

			void SetHDRExposure(float exposure);
			inline float GetHDRExposure() {
				return hdrExposure;
			}

			void SetSSAORadius(float radius);
			inline float GetSSAORadius() {
				return ssaoRadius;
			}

			void SetSSAOBias(float bias);
			inline float GetSSAOBias() {
				return ssaoBias;
			}

			void SetGameWorldMainCamera(int cameraNum) override;

			int GetGameWorldMainCamera() override;

			void SetBossHP(int hp) {
				bossHP = hp;
			}

			void SetPlayerHp(int id, int hp) {
				playersHP[id] = hp;
			}

			void DisplayWinLoseInformation(int playerID) override;
		protected:
			GameTechRenderer();
			~GameTechRenderer() override = default;
		private:
			void SetGameWorldDeltaTime(float dt) override;

			GameWorld& gameWorld;

			std::unique_ptr<PaintingRPass> paintingRPass = nullptr;
			std::unique_ptr<SkyboxRPass>   skyboxPass    = nullptr;
			std::unique_ptr<ModelRPass>    modelPass     = nullptr;
			std::unique_ptr<LightingRPass> lightingPass  = nullptr;
			std::unique_ptr<SSAORPass>     ssaoPass      = nullptr;
			std::unique_ptr<CombineRPass>  combinePass   = nullptr;
			std::unique_ptr<BloomRPass>    bloomPass     = nullptr;
			std::unique_ptr<HDRRPass>      hdrPass       = nullptr;
			std::unique_ptr<PresentRPass>  presentPass   = nullptr;
			std::unique_ptr<DebugRPass>    debugPass     = nullptr;
			std::unique_ptr<MenuRPass>     menuPass      = nullptr;
			std::unique_ptr<HudRPass>      hudPass       = nullptr;

			float gamma = 2.2f;

			Vector3 sunDirection = Vector3(0.0f, -1.0f, 0.0f);

			size_t bloomAmount = 5;
			float bloomBias = 0.04f;

			float hdrExposure = 1.0f;
			
			float ssaoRadius = 0.5f;
			float ssaoBias = 0.025f;
			
			int bossHP = -1;
			int playersHP[4];
		};
	}
}


#endif // !x64

