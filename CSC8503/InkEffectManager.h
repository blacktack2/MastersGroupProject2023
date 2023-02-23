#pragma once
#include "Health.h"
#include "Vector4.h"

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
		using namespace Maths;
		class InkEffectManager {
		public:
			static InkEffectManager& instance() {
				static InkEffectManager INSTANCE;
				return INSTANCE;
			}

			void Update(float dt);
			Vector3 GetColour(paintHell::InkType type);
			void ApplyInkEffect(paintHell::InkType type, Health * health, int healthType);

		protected:
			InkEffectManager();
			~InkEffectManager();

			float damage = 1.0f;
		};
	}
}
