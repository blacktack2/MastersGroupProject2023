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
            Fence,
            Pillar,
            Shelter
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
            bool destroyed = false;
        };

        class GameLevel
        {
        public:
            GameLevel(Vector3 o = Vector3{0,0,0})
            {
                origin = o;
            }

            //~GameLevel(){}

            void AddStuff(Vector3 localPos, ObjectType t)
            {
                stuffs.push_back(GameStuff{ localPos + origin, t });
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
                            AddStuff(Vector3{ x * interval, 0, y * interval }, ObjectType::Fence);
                            break;
                        case '*':
                            AddStuff(Vector3{ x * interval, 0, y * interval }, ObjectType::Shelter);
                            break;
                        case '.':
                            break;
                        default:
                            break;
                        }
                    }
                }
            }

            std::vector<GameStuff>& GetGameStuffs()
            {
                return stuffs;
            }

        protected:
            Vector3 origin{ 0,0,0 };
            std::vector<GameStuff> stuffs;
        };
    }
}