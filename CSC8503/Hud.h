#pragma once
#include"HealthBar.h"
//#define Boss_Origin_HP 100.0f
//#define Player_Origin_HP 100.0f
//#define Boss_X_Scale 0.7f
//#define Boss_Y_Scale 
#include "Assetlibrary.h"
namespace NCL {
	namespace CSC8503 {
		using namespace Maths;
		class Hud
		{
		public:
			Hud() {
			}
			~Hud() {
				/*for (auto hud : huds) {
					delete hud;
				}*/
			}
			void loadHuds(int BossHP,int PlayerHP) {
				HealthBar* BossHealthBar = new HealthBar(AssetLibrary<TextureBase>::GetAsset("BossHealthBar"),Vector2(0.7f * (BossHP / 100.0f-1.0f), -0.8f), Vector2(0.7f * BossHP/ 100.0f, 0.04f));
				HealthBar* BossHealthBarBorder = new HealthBar(AssetLibrary<TextureBase>::GetAsset("BossHealthBarBorder"),Vector2(0, -0.8), Vector2(0.7, 0.04f));
				/*BossHealthBarBorder->SetRenderObject(new MenuRenderObject(BossHealthBarBordertexture));
				BossHealthBar->SetRenderObject(new MenuRenderObject(BossHealthBartexture));*/
				/*TextureBase* PlayerHealthBartexture = AssetLibrary::GetTexture("BossHealthBar");
				TextureBase* PlayerHealthBarBordertexture = AssetLibrary::GetTexture("BossHealthBarBorder");*/
				HealthBar* PlayerHealthBar = new HealthBar(AssetLibrary<TextureBase>::GetAsset("BossHealthBar"), Vector2(-0.6f+0.4f * (PlayerHP / 100.0f - 1.0f), 0.8f), Vector2(0.4f * PlayerHP / 100.0f, 0.03f));
				HealthBar* PlayerHealthBarBorder = new HealthBar(AssetLibrary<TextureBase>::GetAsset("BossHealthBarBorder"), Vector2(-0.6f,0.8f), Vector2(0.4f, 0.03f));
				/*PlayerHealthBarBorder->SetRenderObject(new MenuRenderObject(PlayerHealthBarBordertexture));
				PlayerHealthBar->SetRenderObject(new MenuRenderObject(PlayerHealthBartexture));*/
				huds = { BossHealthBarBorder,BossHealthBar,PlayerHealthBarBorder,PlayerHealthBar};
			};
			const std::vector<HealthBar*>& getHuds() {
				return huds;
			};
		protected:
			std::vector<HealthBar*> huds;
			//Vector2 screenPos;
			//Vector2 dimension;
			//vector<Button*> buttons;
		};
	}
}
