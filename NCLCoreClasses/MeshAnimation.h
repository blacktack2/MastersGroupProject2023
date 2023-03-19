/**
 * @file   MeshAnimation.h
 * @brief  
 * 
 * @author 
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include "Matrix4.h"

#include <optional>
#include <vector>
#include <string>

namespace NCL {
	using namespace NCL::Maths;

	class MeshAnimation {
	public:
		MeshAnimation();

		MeshAnimation(unsigned int jointCount, unsigned int frameCount, float frameRate, std::vector<Matrix4>& frames);

		MeshAnimation(const std::string& filename);
		virtual ~MeshAnimation();

		inline unsigned int GetJointCount() const {
			return jointCount;
		}

		inline unsigned int GetFrameCount() const {
			return frameCount;
		}

		inline float GetFrameRate() const {
			return frameRate;
		}

		/**
		 * @brief Get joint matrices for a specific frame.
		 * @brief Will result in undefined behaviour if frame is greater than
		 * frameCount.
		 */
		const std::vector<Matrix4> GetJointData(unsigned int frame) const;
	private:
		unsigned int jointCount = 0;
		unsigned int frameCount = 0;
		float frameRate = 0.0f;

		std::vector<Matrix4> allJoints{};
	};
}
