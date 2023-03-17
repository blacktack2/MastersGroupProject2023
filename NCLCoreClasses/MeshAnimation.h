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

namespace NCL {
	namespace Maths {
		class Matrix4;
	}
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
		 * @brief Get joint matrices for a specific frame.
		 * @brief Will result in undefined behaviour if frame is greater than
		 * frameCount.
		 */
		const std::vector<Matrix4> GetJointData(unsigned int frame) const;
	protected:
		unsigned int jointCount;
		unsigned int frameCount;
		float frameRate;

		std::vector<Matrix4> allJoints;
	};
}

