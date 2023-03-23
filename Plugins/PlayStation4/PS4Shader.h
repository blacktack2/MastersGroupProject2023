#pragma once
#ifdef _ORBIS
#include "../../Common/ShaderBase.h"
#include "PS4MemoryAware.h"

#include <gnm.h>
#include <gnmx\shaderbinary.h>
#include <gnmx\context.h>

#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <vector>
#include <shader\binary.h>

using std::vector;
using namespace sce;

namespace NCL::PS4 {
	class PS4Shader :
		public Rendering::ShaderBase, public PS4MemoryAware
	{
		friend class PS4RendererBase;
	public:
		~PS4Shader();

		/*void Bind() override;
		void Unbind() override;
		void ReloadShader() override;*/

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
			
		static PS4Shader* GenerateShader(const std::string& vertex, const std::string& pixel);

		int GetConstantVertexBufferIndex(const std::string& name);
		int GetConstantPixelBufferIndex(const std::string& name);

		void SetUniformInt(const std::string& uniform, int v1) override;
		void SetUniformInt(const std::string& uniform, int v1, int v2) override;
		void SetUniformInt(const std::string& uniform, int v1, int v2, int v3) override;
		void SetUniformInt(const std::string& uniform, int v1, int v2, int v3, int v4) override;
		void	SubmitShaderSwitch(Gnmx::GnmxGfxContext& cmdList);
		void SetUniformMatrix(const std::string& uniform, const Matrix2& m) override;
			void SetUniformMatrix(const std::string& uniform, const Matrix3& m) override;
			void SetUniformMatrix(const std::string& uniform, const Matrix4& m) override;
			void SetUniformMatrix(const std::string& uniform, const std::vector<Matrix2>& m) override;
			void SetUniformMatrix(const std::string& uniform, const std::vector<Matrix3>& m) override;
			void SetUniformMatrix(const std::string& uniform, const std::vector<Matrix4>& m) override;

			int GetUniformLocation(const std::string& uniform) override;

	protected:
		PS4Shader();

		void GenerateVertexShader(const std::string& name, bool makeFetch);

		void GeneratePixelShader(const std::string& name);

		void GenerateFetchShader(char* binData);

		bool LoadShaderText(const std::string& name, std::string& into);

		bool LoadShaderBinary(const std::string& name, char*& into, int& dataSize);

		bool ShaderIsBinary(const std::string& name);
		bool LoadSuccess() const {
				return false;
			}

			int GetProgramID() const {
				//return programID;
				return -1;
			}

			/*static std::unique_ptr<ShaderBase> CreateShader(const std::string& vertex, const std::string& fragment);
			static std::unique_ptr<ShaderBase> CreateShaderAndInit(const std::string& vertex, const std::string& fragment);*/

			//void Initialize() override;
	protected:
		void* fetchShader;

		sce::Shader::Binary::Program	vertexBinary;
		Gnmx::VsShader* vertexShader;
		Gnmx::InputOffsetsCache			vertexCache;

		sce::Shader::Binary::Program	pixelBinary;
		Gnmx::PsShader* pixelShader;
		Gnmx::InputOffsetsCache			pixelCache;
	};
}
#endif
