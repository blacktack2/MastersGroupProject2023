/*********************************************************************
 * Author: Xiaoyang Liu
 * Date:   February 2023
 *********************************************************************/

#pragma once
#include "GameObject.h"
#include <fstream>
#include "Assets.h"

namespace NCL
{
    namespace CSC8503
    {
        enum ObjectType
        {
            FenceX,
            FenceY,
            Pillar,
            Shelter,
            Wall
        };

        struct GameStuff
        {
            GameStuff(Vector3 p, ObjectType t)
            {
                worldPos = p;
                objectType = t;
            }

            bool HasDestroyed()
            {
                return destroyed;
            }

            void Destroy(bool b)
            {
                destroyed = b;
            }

            Vector3 worldPos;
            ObjectType objectType;
            bool destroyed = true;      // treat first build as rebuild, to eliminate repeated code
        };

        class GameLevel : public GameObject
        {
        public:
            GameLevel(Vector3 o = Vector3{0,0,0})
            {
                origin = o;
            }

            ~GameLevel(){}

            virtual void Update(float dt) override
            {
                shelterTimer += dt;
            }

            void AddStuff(Vector3 localPos, ObjectType t)
            {
                stuffs.push_back(new GameStuff{ localPos + origin, t });
            }

            void AddRectanglarLevel(const std::string& filename, Vector3 o, float interval)
            {
                origin = o;

                std::ifstream infile(Assets::DATADIR + filename);

                int levelLength = -1;
                int levelWidth = -1;

                infile >> levelLength;
                infile >> levelWidth;

                for (int y = 0; y < levelWidth; ++y) {
                    for (int x = 0; x < levelLength; ++x) {
                        char type = 0;
                        infile >> type;
                        switch (type)
                        {
                        case 'x':
                            AddStuff(Vector3{ x * interval, 0, y * interval }, ObjectType::Pillar);
                            break;
                        case '-':
                            AddStuff(Vector3{ x * interval, 0, y * interval }, ObjectType::FenceX);
                            break;
                        case '|':
                            AddStuff(Vector3{ x * interval, 0, y * interval }, ObjectType::FenceY);
                            break;
                        case '*':
                            AddStuff(Vector3{ x * interval, 0, y * interval }, ObjectType::Shelter);
                            break;
                        case '#':
                            AddStuff(Vector3{ x * interval, 0, y * interval }, ObjectType::Wall);
                            break;
                        case '.':
                            break;
                        default:
                            break;
                        }
                    }
                }
            }

            std::vector<GameStuff*>& GetGameStuffs()
            {
                return stuffs;
            }

            float GetShelterTimer()
            {
                return shelterTimer;
            }

            void SetShelterTimer(float t)
            {
                shelterTimer = t;
            }

        protected:
            Vector3 origin{ 0,0,0 };
            std::vector<GameStuff*> stuffs;
            float shelterTimer = 0.0f;
        };
    }
}