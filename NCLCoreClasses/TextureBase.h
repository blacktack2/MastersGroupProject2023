/**
 * @file   TextureBase.h
 * @brief  Utility class for encapsulating texture functionality.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once

namespace NCL::Rendering {
	class TextureBase {
	public:
		virtual ~TextureBase();

		virtual void Resize(int width, int height) = 0;
		virtual void Bind() = 0;
		virtual void Bind(int slot) = 0;
		virtual void Bind(int slot, int uniform) = 0;
		virtual void Unbind() = 0;
	protected:
		TextureBase();
	};
}