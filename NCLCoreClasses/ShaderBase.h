/**
 * @file   ShaderBase.h
 * @brief  Base shader class and relevant supporting structures.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include <string>

namespace NCL::Rendering {
	enum class ShaderStage {
		Vertex = 0ull,
		Fragment,
		Geometry,
		TessCont,
		TessEval,
		Max,
	};
	class ShaderBase {
	public:
		ShaderBase(const std::string& vert, const std::string& frag, const std::string& tesc, const std::string& tese, const std::string& geom);
		virtual ~ShaderBase();

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void ReloadShader() = 0;
	protected:
		std::string shaderFiles[(size_t)ShaderStage::Max];
	};
}