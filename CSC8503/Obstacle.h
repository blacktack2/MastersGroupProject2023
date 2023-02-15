#pragma once

#include "GameObject.h"
#include "Boss.h"

namespace NCL
{
    namespace CSC8503
    {
        class Obstacle : public GameObject
        {
        public:
            Obstacle()
                : GameObject()
            {
                OnTriggerBeginCallback = [&](GameObject* other)
                {
                    if (!invincible)
                    {
                        if (Bomb* bomb = dynamic_cast<Bomb*>(other))
                        {
                            health -= 1.0f;
                        }
                    }
                    
                };
            }

            //~Obstacle(){}

            virtual void Update(float dt) override
            {
                if (health < 0.0f)
                {
                    Delete();
                }
            }

        protected:
            bool invincible = true;
            float health = 100.0f;
        };
    }
}