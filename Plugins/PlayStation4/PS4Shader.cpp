#ifdef _ORBIS
#include "PS4Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <gnmx\shader_parser.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\shader_loader.h>

using namespace NCL;
using namespace NCL::PS4;
using namespace NCL::Rendering;
using namespace NCL::Maths;

PS4Shader::PS4Shader(PS4Renderer& renderer, const std::string& vertex, const std::string& fragment): renderer(renderer) {

	fetchShader		= nullptr;
	vertexShader	= nullptr;
	pixelShader		= nullptr;


	GenerateVertexShader(vertex, true);
	GeneratePixelShader(fragment);

}


PS4Shader::~PS4Shader()
{
}

std::unique_ptr<ShaderBase> NCL::Rendering::PS4Shader::CreateShader(PS4Renderer& renderer, const std::string& vertex, const std::string& fragment)
{
	return std::make_unique<PS4Shader>(renderer,vertex,fragment);
}

std::unique_ptr<ShaderBase> NCL::Rendering::PS4Shader::CreateShaderAndInit(PS4Renderer& renderer, const std::string& vertex, const std::string& fragment)
{
	std::unique_ptr<PS4Shader> shader=  std::make_unique<PS4Shader>(renderer,vertex, fragment);
	shader->Initialize();
	return shader;
}

void NCL::Rendering::PS4Shader::Initialize(){

}

void NCL::Rendering::PS4Shader::Bind(){
	Gnmx::GnmxGfxContext& cmdList = renderer.GetCurrentGFXContext();
	cmdList.setActiveShaderStages(Gnm::kActiveShaderStagesVsPs);
	cmdList.setVsShader(vertexShader, 0, fetchShader, &vertexCache);
	cmdList.setPsShader(pixelShader, &pixelCache);
}

void NCL::Rendering::PS4Shader::Unbind(){
	
}

void NCL::Rendering::PS4Shader::ReloadShader(){

}

void NCL::Rendering::PS4Shader::SetUniformFloat(const std::string& uniform, float v1){
	int uniformLocation= GetUniformLocation(uniform);
	float* value = (float*)currentGFXContext->allocateFromCommandBuffer(sizeof(float), Gnm::kEmbeddedDataAlignment4);
	*value = v1;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(float));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);

}

void NCL::Rendering::PS4Shader::SetUniformFloat(const std::string& uniform, float v1, float v2){
	int uniformLocation = GetUniformLocation(uniform);
	Vector2 val = Vector2(v1, v2);
	Vector2* value = (Vector2*)currentGFXContext->allocateFromCommandBuffer(sizeof(val), Gnm::kEmbeddedDataAlignment4);
	*value = val;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(val));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformFloat(const std::string& uniform, float v1, float v2, float v3){
	int uniformLocation = GetUniformLocation(uniform);
	Vector3 val = Vector3(v1, v2, v3);
	Vector3* value = (Vector3*)currentGFXContext->allocateFromCommandBuffer(sizeof(val), Gnm::kEmbeddedDataAlignment4);
	*value=val;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(val));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformFloat(const std::string& uniform, float v1, float v2, float v3, float v4){
	int uniformLocation = GetUniformLocation(uniform);
	Vector4 val = Vector4(v1, v2, v3, v4);
	Vector4* value = (Vector4*)currentGFXContext->allocateFromCommandBuffer(sizeof(val), Gnm::kEmbeddedDataAlignment4);
	*value = val;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(val));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformFloat(const std::string& uniform, const Vector2& val) {
	int uniformLocation = GetUniformLocation(uniform);
	Vector2* value = (Vector2*)currentGFXContext->allocateFromCommandBuffer(sizeof(val), Gnm::kEmbeddedDataAlignment4);
	*value = val;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(val));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformFloat(const std::string& uniform, const Vector3& val){
	int uniformLocation = GetUniformLocation(uniform);
	Vector3* value = (Vector3*)currentGFXContext->allocateFromCommandBuffer(sizeof(val), Gnm::kEmbeddedDataAlignment4);
	*value = val;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(val));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformFloat(const std::string& uniform, const Vector4& val){
	int uniformLocation = GetUniformLocation(uniform);
	Vector4* value = (Vector4*)currentGFXContext->allocateFromCommandBuffer(sizeof(val), Gnm::kEmbeddedDataAlignment4);
	*value = val;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(val));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformFloat(const std::string& uniform, const std::vector<Vector2>& val){
	size_t size = sizeof(*val.data()) * data.size();
	int uniformLocation = GetUniformLocation(uniform);
	float* value = (float*)currentGFXContext->allocateFromCommandBuffer(size, Gnm::kEmbeddedDataAlignment4);
	memcpy(value, val.data(), size);
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, size);
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}
void NCL::Rendering::PS4Shader::SetUniformFloat(const std::string& uniform, const std::vector<Vector3>& val) {
	size_t size = sizeof(*val.data()) * data.size();
	int uniformLocation = GetUniformLocation(uniform);
	float* value = (float*)currentGFXContext->allocateFromCommandBuffer(size, Gnm::kEmbeddedDataAlignment4);
	memcpy(value, val.data(), size);
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, size);
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}
void NCL::Rendering::PS4Shader::SetUniformFloat(const std::string& uniform, const std::vector<Vector4>& val) {
	size_t size = sizeof(*val.data()) * data.size();
	int uniformLocation = GetUniformLocation(uniform);
	float* value = (float*)currentGFXContext->allocateFromCommandBuffer(size, Gnm::kEmbeddedDataAlignment4);
	memcpy(value, val.data(), size);
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, size);
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformInt(const std::string& uniform, int v1) {
	int uniformLocation = GetUniformLocation(uniform);
	int* value = (int*)currentGFXContext->allocateFromCommandBuffer(sizeof(int), Gnm::kEmbeddedDataAlignment4);
	*value = v1;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(int));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformInt(const std::string& uniform, int v1, int v2) {
	int uniformLocation = GetUniformLocation(uniform);
	Vector2i val = Vector2i(v1, v2);
	Vector2i* value = (Vector2i*)currentGFXContext->allocateFromCommandBuffer(sizeof(val), Gnm::kEmbeddedDataAlignment4);
	*value = val;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(val));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformInt(const std::string& uniform, int v1, int v2, int v3) {
	int uniformLocation = GetUniformLocation(uniform);
	Vector3i val = Vector3i(v1, v2, v3);
	Vector3i* value = (Vector3i*)currentGFXContext->allocateFromCommandBuffer(sizeof(val), Gnm::kEmbeddedDataAlignment4);
	*value = val;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(val));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformInt(const std::string& uniform, int v1, int v2, int v3, int v4) {
	int uniformLocation = GetUniformLocation(uniform);
	Vector4i val = Vector4i(v1, v2, v3, v4);
	Vector4i* value = (Vector4i*)currentGFXContext->allocateFromCommandBuffer(sizeof(val), Gnm::kEmbeddedDataAlignment4);
	*value = val;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(val));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformMatrix(const std::string& uniform, const Matrix2& m) {
	int uniformLocation = GetUniformLocation(uniform);
	Matrix2* value = (Matrix2*)currentGFXContext->allocateFromCommandBuffer(sizeof(m), Gnm::kEmbeddedDataAlignment4);
	*value = m;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(m));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformMatrix(const std::string& uniform, const Matrix3& m) {
	int uniformLocation = GetUniformLocation(uniform);
	Matrix3* value = (Matrix3*)currentGFXContext->allocateFromCommandBuffer(sizeof(m), Gnm::kEmbeddedDataAlignment4);
	*value = m;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(m));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformMatrix(const std::string& uniform, const Matrix4& m) {
	int uniformLocation = GetUniformLocation(uniform);
	Matrix4* value = (Matrix4*)currentGFXContext->allocateFromCommandBuffer(sizeof(m), Gnm::kEmbeddedDataAlignment4);
	*value = m;
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, sizeof(m));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

void NCL::Rendering::PS4Shader::SetUniformMatrix(const std::string& uniform, const std::vector<Matrix2>& m) {
	size_t size = sizeof(*m.data()) * m.size();
	int uniformLocation = GetUniformLocation(uniform);
	Matrix2* value = (Matrix2*)currentGFXContext->allocateFromCommandBuffer(size, Gnm::kEmbeddedDataAlignment4);
	memcpy(value, m.data(), size);
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, size);
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}
void NCL::Rendering::PS4Shader::SetUniformMatrix(const std::string& uniform, const std::vector<Matrix3>& m) {
	size_t size = sizeof(*m.data()) * m.size();
	int uniformLocation = GetUniformLocation(uniform);
	Matrix3* value = (Matrix3*)currentGFXContext->allocateFromCommandBuffer(size, Gnm::kEmbeddedDataAlignment4);
	memcpy(value, m.data(), size);
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, size);
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}
void NCL::Rendering::PS4Shader::SetUniformMatrix(const std::string& uniform, const std::vector<Matrix4>& m) {
	size_t size = sizeof(*m.data()) * m.size();
	int uniformLocation = GetUniformLocation(uniform);
	Matrix4* value = (Matrix4*)currentGFXContext->allocateFromCommandBuffer(size, Gnm::kEmbeddedDataAlignment4);
	memcpy(value, m.data(), size);
	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(value, size);
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStageVs, uniformLocation, 1, &constantBuffer);
	renderer.GetCurrentGFXContext()->setConstantBuffers(Gnm::kShaderStagePs, uniformLocation, 1, &constantBuffer);
}

int NCL::Rendering::PS4Shader::GetUniformLocation(const std::string& uniform) {
	sce::Shader::Binary::Buffer* constantBuffer = vertexBinary.getBufferResourceByName(name.c_str());
	return constantBuffer ? constantBuffer->m_resourceIndex : -1;

}

void PS4Shader::GenerateVertexShader(const std::string&name, bool makeFetch) {
	char*	binData = NULL;	//resulting compiled shader bytes
	int		binSize = 0;
	Gnmx::ShaderInfo shaderInfo;

	if (ShaderIsBinary(name)) {
		if (LoadShaderBinary(name, binData, binSize)) {		
			vertexBinary.loadFromMemory(binData, binSize);

			//sce::Shader::Binary::Buffer* constantBuffer = vertexBinary.getBufferResourceByName("LOLBUFFER");

			Gnmx::parseShader(&shaderInfo, binData);

			void* shaderBinary = garlicAllocator->allocate(shaderInfo.m_gpuShaderCodeSize, Gnm::kAlignmentOfShaderInBytes);
			void* shaderHeader = onionAllocator->allocate(shaderInfo.m_vsShader->computeSize(), Gnm::kAlignmentOfBufferInBytes);

			memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
			memcpy(shaderHeader, shaderInfo.m_vsShader, shaderInfo.m_vsShader->computeSize());

			vertexShader = (Gnmx::VsShader*)shaderHeader;
			vertexShader->patchShaderGpuAddress(shaderBinary);
		}
		else {
			std::cout << "Failed to generate vertex shader: " << name << " from binary. " << std::endl;
			return;
		}
	}
	else {
		std::string shaderString;
		if (LoadShaderText(name, shaderString)) {
			//shaderString now contains the pssl shader data
			//MAGIC GOES HERE
		}
		else {
			std::cout << "Failed to generate vertex shader: " << name << " from raw text. " << std::endl;
			return;
		}
	}

	sce::Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
							shaderInfo.m_gpuShaderCodeSize, name.c_str(), Gnm::kResourceTypeShaderBaseAddress, 0);

	sce::Gnmx::generateInputResourceOffsetTable(&vertexCache, Gnm::kShaderStageVs, vertexShader);

	if (makeFetch) {
		GenerateFetchShader(binData);
	}
	//delete binData;//done with bindata now!
}

void PS4Shader::GenerateFetchShader(char* binData) {
	fetchShader = garlicAllocator->allocate(Gnmx::Toolkit::calculateMemoryRequiredForVsFetchShader(binData));

	uint32_t shaderModifier;
	Gnmx::generateVsFetchShader(fetchShader, &shaderModifier, vertexShader);

	vertexShader->applyFetchShaderModifier(shaderModifier);
}

void PS4Shader::GeneratePixelShader(const std::string&name) {
	char*	binData = NULL;	//resulting compiled shader bytes
	int		binSize = 0;
	Gnmx::ShaderInfo shaderInfo;

	if (ShaderIsBinary(name)) {
		if (LoadShaderBinary(name, binData, binSize)) {
			pixelBinary.loadFromMemory(binData, binSize);
			Gnmx::parseShader(&shaderInfo, binData);

			void* shaderBinary = garlicAllocator->allocate(shaderInfo.m_gpuShaderCodeSize, Gnm::kAlignmentOfShaderInBytes);
			void* shaderHeader = onionAllocator->allocate(shaderInfo.m_psShader->computeSize(), Gnm::kAlignmentOfBufferInBytes);

			memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
			memcpy(shaderHeader, shaderInfo.m_psShader, shaderInfo.m_psShader->computeSize());

			pixelShader = (Gnmx::PsShader*)shaderHeader;
			pixelShader->patchShaderGpuAddress(shaderBinary);
		}
	}
	else {
		bool a = true;
	}

	Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
		shaderInfo.m_gpuShaderCodeSize, name.c_str(), Gnm::kResourceTypeShaderBaseAddress, 0);

	Gnmx::generateInputOffsetsCache(&pixelCache, Gnm::kShaderStagePs, pixelShader);

	//delete binData;
}



bool PS4Shader::LoadShaderText(const std::string &name, std::string&into) {

	return false;
}

bool PS4Shader::LoadShaderBinary(const std::string &name, char*& into, int& dataSize) {
	std::ifstream binFile(name, std::ios::binary);

	if (!binFile) {
		return false;
	}
	binFile.seekg(0, ios::end);
	int size = binFile.tellg();

	into = new char[size];

	binFile.seekg(0, ios::beg);
	binFile.read(into, size);

	dataSize = size;

	return true;
}

bool PS4Shader::ShaderIsBinary(const std::string& name) {
	if (name.length() >= 3 && 
		name[name.length() - 3] == '.' &&
		name[name.length() - 2] == 's' &&
		name[name.length() - 1] == 'b'	) {
		return true;
	}
	return false;
}

void	PS4Shader::SubmitShaderSwitch(Gnmx::GnmxGfxContext& cmdList) {
	cmdList.setActiveShaderStages(Gnm::kActiveShaderStagesVsPs);

	cmdList.setVsShader(vertexShader, 0, fetchShader, &vertexCache);
	cmdList.setPsShader(pixelShader, &pixelCache);
}

int		PS4Shader::GetConstantBufferIndex(const std::string &name) {
	
}

#endif