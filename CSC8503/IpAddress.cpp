#include "IpAddress.h"


using namespace NCL;
using namespace CSC8503;

IpAddress::IpAddress() {
	Default();
}

IpAddress::~IpAddress() {
}

void NCL::CSC8503::IpAddress::AddDigit(int num)
{
	if (index < 4) {
		if (address[index].size() >= 3) {
			if (std::stoi(address[index]) > 255) {
				address[index] = "255";
			}
			NextIndex();
		}
	}
	if(index < 4){
		address[index] += std::to_string(num);
	}
}

void NCL::CSC8503::IpAddress::RemoveDigit()
{
	if (index == 4) {
		PrevIndex();
	}
	if (index >= 0) {
		if (address[index].size() == 0) {
			PrevIndex();
		}
		else {
			address[index].pop_back();
		}
	}
	
}

void NCL::CSC8503::IpAddress::NextIndex()
{
	if (index == 4) {
		return;
	}
	index++;
}

void NCL::CSC8503::IpAddress::PrevIndex()
{
	if (index == 0) {
		return;
	}
	index--;
}

void NCL::CSC8503::IpAddress::Clear()
{
	for (int i = 0; i < 4; i++) {
		address[i].clear();
	}
}

void NCL::CSC8503::IpAddress::Default()
{
	address[0] = "127";
	address[1] = "0";
	address[2] = "0";
	address[3] = "1";
	index = 4;
}

