#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class BonusObject : public GameObject {
		public:
			BonusObject();
			~BonusObject();

			virtual void Update(float dt) override;
		};
	}
}