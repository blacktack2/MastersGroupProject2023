#pragma once
#include "GameObject.h"

#include <string>

namespace NCL {
    namespace CSC8503 {
		class GameWorld;
        class StateMachine;

        class StateGameObject : public GameObject  {
        public:
            StateGameObject(GameWorld& gameWorld, std::string name = "");
            ~StateGameObject();

            virtual void Update(float dt);

			StateMachine* GetStateMachine() {
				return stateMachine;
			}
        protected:
            void MoveLeft(float dt);
            void MoveRight(float dt);

            StateMachine* stateMachine;
            float counter;
        };
    }
}
