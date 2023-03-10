#pragma once
#include "OGLOverlayRenderPass.h"
#include "GameWorld.h"
#include "UIObject.h"
#include"HealthBar.h"
#include"OGLTexture.h"
#include"OGLShader.h"
namespace NCL::Rendering {
	class OGLShader;
	class OGLTexture;
}

using namespace NCL::Rendering;

namespace NCL::CSC8503 {
	class GameTechRenderer;
	class HudRPass : public OGLOverlayRenderPass {
	public:
		HudRPass();
		//HudRPass(OGLRenderer& renderer, GameWorld& gameWorld);
		~HudRPass();

		virtual void Render() override;
		void SetHud(std::vector<HealthBar*> hud) {
			huds = hud;
		};
		void SetBossHealthBar(HealthBar* healthbar) {
			BossHealth = healthbar;
		};
		void SetBossHealthBarBorder(HealthBar* healthbarBorder) {
			BossBorder = healthbarBorder;
		};
	private:
		void DrawHud();
		//void DrawButtons();
		void DrawUIObject(UIObject* obj);
		GameTechRenderer& renderer;
		GameWorld& gameWorld;
		std::unique_ptr<ShaderBase> defaultShader;
		std::unique_ptr<TextureBase> defaultTexture;
		HealthBar* BossHealth;
		HealthBar* BossBorder;
		std::vector<HealthBar*> huds;
	};
}