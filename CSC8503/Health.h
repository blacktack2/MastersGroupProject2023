#pragma once

namespace NCL {
	namespace CSC8503 {
		class Health{
		public:
			Health();
			Health(float health);
			~Health();

			void Update(float dt);

			const float GetHealth() {
				return health;
			}
			void Damage(float damage) {
				if (health <= 0)
					return;
				health -= damage;
				if (health < 0)
					health = 0;
			}
			void Heal(float heal) {
				health += heal;
			}
			void DamageOverTime(float damage, float time) {
				if (overTimeTimer <= 0) {
					Damage(damage);
					overTimeTimer = time;
				}
			}
		protected:
			float health;
			float overTimeTimer = 0;
		};
	}
}
