/**
 * @file   IRenderPass.h
 * @brief  Top-most Interface class representing a single pass in the rendering
 * pipeline.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once

namespace NCL {
	namespace Rendering {
		/**
		 * @brief Top-most Interface class representing a single pass in the
		 * rendering pipeline.
		 */
		class IRenderPass {
		public:
			virtual void OnWindowResize(int width, int height) = 0;
			virtual void Render() = 0;
		};
	}
}
