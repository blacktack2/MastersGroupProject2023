#pragma once
#ifdef _ORBIS
#include "ShaderBase.h"
#include "PS4MemoryAware.h"
#include "PS4Renderer.h"

#include <gnm.h>
#include <gnmx\shaderbinary.h>
#include <gnmx\context.h>

#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <vector>
#include <shader\binary.h>
#include <memory>
#include <unordered_map>
#include <PS4Mesh.cpp>

using std::vector;

namespace NCL {
	using namespace Maths;
	namespace Rendering {
		class PS4Shader :
			public ShaderBase, public PS4::PS4MemoryAware
		{
			friend class PS4RendererBase;
		public:
			~PS4Shader() override;

			/**
			 * @brief Create a new shader from the given asset files.
			 * @brief See PS4Shader::PS4Shader().
			 */
			static std::unique_ptr<ShaderBase> CreateShader(PS4Renderer& renderer,const std::string& vertex, const std::string& fragment);
			/**
			 * @brief Create a new shader from the given asset files and call Initialize.
			 * @brief  See PS4Shader::PS4Shader().
			 */
			static std::unique_ptr<ShaderBase> CreateShaderAndInit(PS4Renderer& renderer, const std::string& vertex, const std::string& fragment);


			//void	SubmitShaderSwitch(sce::Gnmx::GnmxGfxContext& cmdList);

			void Initialize() override;

			void Bind() override;
			void Unbind() override;
			void ReloadShader() override;

			void SetUniformFloat(const std::string& uniform, float v1) override;
			void SetUniformFloat(const std::string& uniform, float v1, float v2) override;
			void SetUniformFloat(const std::string& uniform, float v1, float v2, float v3) override;
			void SetUniformFloat(const std::string& uniform, float v1, float v2, float v3, float v4) override;
			void SetUniformFloat(const std::string& uniform, const Vector2& v) override;
			void SetUniformFloat(const std::string& uniform, const Vector3& v) override;
			void SetUniformFloat(const std::string& uniform, const Vector4& v) override;
			void SetUniformFloat(const std::string& uniform, const std::vector<Vector2>& v) override;
			void SetUniformFloat(const std::string& uniform, const std::vector<Vector3>& v) override;
			void SetUniformFloat(const std::string& uniform, const std::vector<Vector4>& v) override;

			void SetUniformInt(const std::string& uniform, int v1) override;
			void SetUniformInt(const std::string& uniform, int v1, int v2) override;
			void SetUniformInt(const std::string& uniform, int v1, int v2, int v3) override;
			void SetUniformInt(const std::string& uniform, int v1, int v2, int v3, int v4) override;

			void SetUniformMatrix(const std::string& uniform, const Matrix2& m) override;
			void SetUniformMatrix(const std::string& uniform, const Matrix3& m) override;
			void SetUniformMatrix(const std::string& uniform, const Matrix4& m) override;
			void SetUniformMatrix(const std::string& uniform, const std::vector<Matrix2>& m) override;
			void SetUniformMatrix(const std::string& uniform, const std::vector<Matrix3>& m) override;
			void SetUniformMatrix(const std::string& uniform, const std::vector<Matrix4>& m) override;

			int GetUniformLocation(const std::string& uniform) override;


		protected:
			PS4Shader(PS4Renderer& renderer, const std::string& vertex, const std::string& fragment);

			void GenerateVertexShader(const std::string& name, bool makeFetch);

			void GeneratePixelShader(const std::string& name);

			void GenerateFetchShader(char* binData);

			bool LoadShaderText(const std::string& name, std::string& into);

			bool LoadShaderBinary(const std::string& name, char*& into, int& dataSize);

			bool ShaderIsBinary(const std::string& name);

			

		protected:
			void* fetchShader;

			PS4Renderer& renderer;

			sce::Shader::Binary::Program	vertexBinary;
			Gnmx::VsShader* vertexShader;
			Gnmx::InputOffsetsCache			vertexCache;

			sce::Shader::Binary::Program	pixelBinary;
			Gnmx::PsShader* pixelShader;
			Gnmx::InputOffsetsCache			pixelCache;
		};
	}
}
#endif
