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
			float GetMaxHealth() {
				return maxHealth;
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
				if (health > maxHealth)
					health = maxHealth;
			}
			void DamageOverTime(float damage, float time) {
				if (overTimeTimer <= 0) {
					Damage(damage);
					overTimeTimer = time;
				}
			}

			void SetHealth(float health) {
				this->health = health;
				if (this->health > maxHealth)
					this->health = maxHealth;
			}
		protected:
			float health;
			float maxHealth;
			float overTimeTimer = 0;
		};
	}
}
