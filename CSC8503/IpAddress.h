#pragma once

#include <string>
#include <iostream>
namespace NCL {
	namespace CSC8503 {
		class IpAddress {
		public:
			IpAddress();
			~IpAddress();

			void AddDigit(int);
			void RemoveDigit();
			void NextIndex();
			void PrevIndex();
			void Clear();
			void Default();
			void Print() {
				for (int i = 0; i < 4; i++) {
					if (address[i].size() == 0) {
						std::cout << "0" << ".";
					}else
						std::cout << address[i] << ".";
				}
				std::cout << std::endl;
			}
			int GetAddress(int index) {
				if (index > 3) {
					return 0;
				}
				if (address[index].size() == 0) {
					return 0;
				}
				return stoi(address[index]);
			}
		protected:
			std::string address[4];
			short int index = 0;
		};
	}
}
