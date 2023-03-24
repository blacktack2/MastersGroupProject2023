#include "OptionManager.h"

using namespace NCL;
using namespace CSC8503;

OptionManager::OptionManager(){
	address = IpAddress();
}

OptionManager::~OptionManager() {
}

void OptionManager::Update(float dt) {
}

IpAddress& OptionManager::GetIpAddress() {
	return address;
}

