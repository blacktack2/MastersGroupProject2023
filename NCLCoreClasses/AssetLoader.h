/**
 * @file   AssetLoader.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include <map>
#include <memory>
#include <functional>
#include <string>

namespace NCL::Rendering {
	class BufferObjectBase;
	class FrameBuffer;
	class ShaderBase;
	class TextureBase;
	enum class TextureType;
}

using namespace NCL::Rendering;

namespace NCL {
	class MeshGeometry;

	typedef std::function<bool(const std::string& filename, char*& outData, unsigned int& width, unsigned int &height, int &channels, int&flags)> TextureDataLoadFunction;

	typedef std::function<std::unique_ptr<TextureBase>(const std::string& filename)> TextureLoadFunction;
	typedef std::function<std::unique_ptr<MeshGeometry>(const std::string& filename)> MeshLoadFunction;

	typedef std::function<std::unique_ptr<BufferObjectBase>(size_t size, unsigned int binding)> BufferObjectCreateFunction;
	typedef std::function<std::unique_ptr<FrameBuffer>()> FrameBufferCreateFunction;
	typedef std::function<std::unique_ptr<MeshGeometry>()> MeshCreateFunction;
	typedef std::function<std::unique_ptr<ShaderBase>(const std::string& vertex, const std::string& fragment)> ShaderCreateFunction;
	typedef std::function<std::unique_ptr<TextureBase>(TextureType type, unsigned int width, unsigned int height)> TextureCreateFunction;

	class AssetLoader {
	public:
		static void RegisterTextureDataLoadFunction(TextureDataLoadFunction f, const std::string& fileExtension);

		static void RegisterMeshLoadFunction(MeshLoadFunction f);
		static void RegisterTextureLoadFunction(TextureLoadFunction f);

		static void RegisterBufferObjectCreateFunction(BufferObjectCreateFunction f);
		static void RegisterFrameBufferCreateFunction(FrameBufferCreateFunction f);
		static void RegisterMeshCreateFunction(MeshCreateFunction f);
		static void RegisterShaderCreateFunction(ShaderCreateFunction f);
		static void RegisterShaderCreateAndInitFunction(ShaderCreateFunction f);
		static void RegisterTextureCreateFunction(TextureCreateFunction f);

		static bool LoadTextureData(const std::string& filename, char*& outData, unsigned int& width, unsigned int &height, int &channels, int&flags);

		static std::unique_ptr<MeshGeometry> LoadMesh(const std::string& filename);
		static std::unique_ptr<TextureBase>  LoadTexture(const std::string& filename);

		static std::unique_ptr<BufferObjectBase> CreateBufferObject(size_t size, unsigned int binding);
		static std::unique_ptr<FrameBuffer>      CreateFrameBuffer();
		static std::unique_ptr<MeshGeometry>     CreateMesh();
		static std::unique_ptr<ShaderBase>       CreateShader(const std::string& vertex, const std::string& fragment);
		static std::unique_ptr<ShaderBase>       CreateShaderAndInit(const std::string& vertex, const std::string& fragment);
		static std::unique_ptr<TextureBase>      CreateTexture(TextureType type, unsigned int width, unsigned int height);
	protected:
		static std::string GetFileExtension(const std::string& fileExtension);

		static std::map<std::string, TextureDataLoadFunction> fileHandlers;

		static MeshLoadFunction     meshLoad;
		
		static TextureLoadFunction  textureLoad;

		static BufferObjectCreateFunction bufferObjectCreate;
		static FrameBufferCreateFunction  frameBufferCreate;
		static MeshCreateFunction         meshCreate;
		static ShaderCreateFunction       shaderCreate;
		static ShaderCreateFunction       shaderCreateAndInit;
		static TextureCreateFunction      textureCreate;
	};
}
