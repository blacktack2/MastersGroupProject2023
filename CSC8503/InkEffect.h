#pragma once

namespace NCL {
	namespace CSC8503 {
		class InkEffect{
		public:
			InkEffect();
			~InkEffect();

			void Update(float dt);

		protected:
			float damage = 1.0f;
		};
	}
}
