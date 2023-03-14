/**
 * @file   Hud.cpp
 * @brief  See Hud.h.
 *
 * @author Shantao Liu
 * @author Felix Chiu
 * @date   March 2023
 */

#include "Hud.h"
using namespace NCL;
using namespace CSC8503;
using namespace Maths;

void Hud::loadHuds(int BossHP, int PlayerHP) {
	/*
	healthbar* bosshealthbar = new healthbar(assetlibrary<texturebase>::getasset("bosshealthbar"), vector2(0.7f * (bosshp / 100.0f - 1.0f), -0.8f), vector2(0.7f * bosshp / 100.0f, 0.04f));
	healthbar* bosshealthbarborder = new healthbar(assetlibrary<texturebase>::getasset("bosshealthbarborder"), vector2(0, -0.8), vector2(0.7, 0.04f));
	healthbar* playerhealthbar = new healthbar(assetlibrary<texturebase>::getasset("playerhealthbar"), vector2(-0.6f + 0.4f * (playerhp / 100.0f - 1.0f), 0.8f), vector2(0.4f * playerhp / 100.0f, 0.03f));
	healthbar* playerhealthbarborder = new healthbar(assetlibrary<texturebase>::getasset("playerhealthbarborder"), vector2(-0.6f, 0.8f), vector2(0.4f, 0.03f));
	huds = { bosshealthbarborder,bosshealthbar,playerhealthbarborder,playerhealthbar };
	*/
};

void  Hud::AddHuds(Health* hp, Vector2 anchor, Vector2 dimension) {
	HealthBar* NewHealthBar = new HealthBar(AssetLibrary<TextureBase>::GetAsset("PlayerHealthBar"), anchor, dimension, hp);
	huds.push_back(NewHealthBar);
	HealthBar* NewHealthBarborder = new HealthBar(AssetLibrary<TextureBase>::GetAsset("PlayerHealthBarBorder"), anchor, dimension);
	huds.push_back(NewHealthBarborder);
}