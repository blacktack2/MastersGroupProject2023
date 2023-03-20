#include "ShaderBase.h"
using namespace NCL;
using namespace Rendering;

ShaderBase::ShaderBase(const std::string& vertex, const std::string& fragment, const std::string& geometry, const std::string& domain, const std::string& hull)
{
	shaderFiles[(int)ShaderStages::SHADER_VERTEX]	= vertex;
	shaderFiles[(int)ShaderStages::SHADER_FRAGMENT]	= fragment;
	shaderFiles[(int)ShaderStages::SHADER_GEOMETRY]	= geometry;
	shaderFiles[(int)ShaderStages::SHADER_DOMAIN]	= domain;
	shaderFiles[(int)ShaderStages::SHADER_HULL]		= hull;
}

ShaderBase::~ShaderBase()
{
}