#pragma once
#ifdef _ORBIS
#include "ShaderBase.h"
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

namespace NCL {
	namespace PS4 {
		class PS4Shader :
			public Rendering::ShaderBase, public PS4MemoryAware
		{
			friend class PS4RendererBase;
		public:
			~PS4Shader();

			static PS4Shader* GenerateShader(const std::string& vertex, const std::string& pixel);

			int		GetConstantBufferIndex(const std::string& name);
			void	SubmitShaderSwitch(Gnmx::GnmxGfxContext& cmdList);

		protected:
			PS4Shader();

			void ReloadShader() override {
				//we load binaries in directly...
			}

			void GenerateVertexShader(const std::string& name, bool makeFetch);

			void GeneratePixelShader(const std::string& name);

			void GenerateFetchShader(char* binData);

			bool LoadShaderText(const std::string& name, std::string& into);

			bool LoadShaderBinary(const std::string& name, char*& into, int& dataSize);

			bool ShaderIsBinary(const std::string& name);

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
}
#endif
