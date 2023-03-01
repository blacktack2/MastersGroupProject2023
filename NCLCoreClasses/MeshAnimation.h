/**
 * @file   MeshAnimation.h
 * @brief  
 * 
 * @author 
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include <optional>
#include <vector>
#include <string>

namespace NCL::Maths {
	class Matrix4;
}

using namespace NCL::Maths;

namespace NCL {
	class MeshAnimation {
	public:
		MeshAnimation();

		MeshAnimation(unsigned int jointCount, unsigned int frameCount, float frameRate, std::vector<Maths::Matrix4>& frames);

		MeshAnimation(const std::string& filename);
		virtual ~MeshAnimation();

		unsigned int GetJointCount() const {
			return jointCount;
		}

		unsigned int GetFrameCount() const {
			return frameCount;
		}

		float GetFrameRate() const {
			return frameRate;
		}

		/**
		 * @brief .
		 * @brief Will result in undefined behaviour if frame.
		 * 
		 * @param frame
		 * @return 
		 */
		const std::vector<Matrix4> GetJointData(unsigned int frame) const;
	protected:
		unsigned int jointCount;
		unsigned int frameCount;
		float frameRate;

		std::vector<Matrix4> allJoints;
	};
}

