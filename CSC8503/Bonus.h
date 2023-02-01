#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class BonusObject : public GameObject {
		public:
			BonusObject(GameWorld& gameWorld);
			~BonusObject();

			virtual void Update(float dt) override;
		};
	}
}