#pragma once
#include "Health.h"

namespace paintHell {
	enum InkType
	{
		NoInk,			// No effect
		BossDamage,			// from boss causing damage
		PlayerDamage,			// from player
	};
}
namespace NCL {
	namespace CSC8503 {
		
		class InkEffectManager {
		public:
			static InkEffectManager& instance() {
				static InkEffectManager INSTANCE;
				return INSTANCE;
			}

			void Update(float dt);
			void ApplyInkEffect(paintHell::InkType type, Health * health, int healthType);
		protected:
			InkEffectManager();
			~InkEffectManager();

			float damage = 1.0f;
		};
	}
}
