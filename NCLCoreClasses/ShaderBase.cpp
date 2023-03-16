/**
 * @file   ShaderBase.cpp
 * @brief  See ShaderBase.h
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "ShaderBase.h"

using namespace NCL::Rendering;

ShaderBase::ShaderBase(const std::string& vert, const std::string& frag, const std::string& tesc, const std::string& tese, const std::string& geom) {
	shaderFiles[(size_t)ShaderStage::Vertex] = vert;
	shaderFiles[(size_t)ShaderStage::Fragment] = frag;
	shaderFiles[(size_t)ShaderStage::TessCont] = tesc;
	shaderFiles[(size_t)ShaderStage::TessEval] = tese;
	shaderFiles[(size_t)ShaderStage::Geometry] = geom;
}
