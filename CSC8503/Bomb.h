/*********************************************************************
 * Author: Xiaoyang Liu
 * Date:   February 2023
 *********************************************************************/

#pragma once
#include "PlayerObject.h";
#include "Obstacle.h";

namespace NCL
{
    namespace CSC8503
    {
        //class BossBehaviorTree;

        class Bomb : public GameObject
        {
        public:
            Bomb(Vector3 v)
                : GameObject()
            {
                velocity = v;

                OnTriggerBeginCallback = [&](GameObject* other)
                {
                    if (PlayerObject* player = dynamic_cast<PlayerObject*>(other)) {
                        lifeSpan = -1.0f;
                    }
                    if (Obstacle* obstacle = dynamic_cast<Obstacle*>(other)) {
                        lifeSpan = -1.0f;
                    }
                };
            }

            //~Bomb(){}

            virtual void Update(float dt) override
            {
                this->GetTransform().SetPosition(this->GetTransform().GetGlobalPosition() + velocity * dt);
                lifeSpan -= dt;
                if (lifeSpan < 0.0f)
                {
                    Delete();
                }
                if (this->GetTransform().GetGlobalPosition().y < 1.0f)
                {
                    GetTransform().SetPosition({ GetTransform().GetGlobalPosition().x, 1.0f, GetTransform().GetGlobalPosition().z });
                }
            }

        protected:
            Vector3 velocity{ 0,0,0 };
            float lifeSpan = 5.0f;
        };
    }
}
