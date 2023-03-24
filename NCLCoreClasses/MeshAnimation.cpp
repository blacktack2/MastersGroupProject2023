/**
 * @file   MeshAnimation.cpp
 * @brief  See MeshAnimation.h.
 * 
 * @author 
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "MeshAnimation.h"

#include "Assets.h"

#include <fstream>
#include <string>

using namespace NCL;
using namespace Maths;

MeshAnimation::MeshAnimation() {
}

MeshAnimation::MeshAnimation(unsigned int jointCount, unsigned int frameCount, float frameRate, std::vector<Matrix4>& frames) {
	this->jointCount = jointCount;
	this->frameCount = frameCount;
	this->frameRate  = frameRate;
	this->allJoints  = frames;
}

MeshAnimation::MeshAnimation(const std::string& filename) : MeshAnimation() {
	std::ifstream file(Assets::MESHDIR + filename);

	std::string filetype;
	int fileVersion;

	file >> filetype;

	if (filetype != "MeshAnim") {
		//std::cout << __FUNCTION__ << " File is not a MeshAnim file!\n";
		return;
	}
	file >> fileVersion;
	file >> frameCount;
	file >> jointCount;
	file >> frameRate;

	allJoints.reserve((size_t)frameCount * (size_t)jointCount);

	for (unsigned int frame = 0; frame < frameCount; ++frame) {
		for (unsigned int joint = 0; joint < jointCount; ++joint) {
			Matrix4 mat;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					file >> mat.array[i][j];
				}
			}
			allJoints.emplace_back(mat);
		}
	}
}

MeshAnimation::~MeshAnimation() {
}

const std::vector<Matrix4> MeshAnimation::GetJointData(unsigned int frame) const {
	int matStart = (frame    ) * jointCount;
	int matEnd   = (frame + 1) * jointCount;

	return std::vector<Matrix4>(allJoints.begin() + matStart, allJoints.begin() + matEnd);
}
