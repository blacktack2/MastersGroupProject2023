#pragma once
#include <string>
#include <vector>

namespace NCL{
	namespace Maths {
		class Vector2;
		class Vector3;
		class Vector4;
		class Matrix2;
		class Matrix3;
		class Matrix4;
	}
}

using namespace NCL::Maths;

namespace NCL {
	namespace Rendering {
		enum class ShaderStages {
			SHADER_VERTEX,
			SHADER_FRAGMENT,
			SHADER_GEOMETRY,
			SHADER_DOMAIN,
			SHADER_HULL,
			SHADER_MAX
		};

		class ShaderBase	{
		public:
			ShaderBase() {
			}
			ShaderBase(const std::string& vertex, const std::string& fragment, const std::string& geometry = "", const std::string& domain = "", const std::string& hull = "");
			virtual ~ShaderBase();

			//virtual void Bind() = 0;
			//virtual void Unbind() = 0;
			//virtual void ReloadShader() = 0;

			virtual void SetUniformFloat(const std::string& uniform, float v1) = 0;
			virtual void SetUniformFloat(const std::string& uniform, float v1, float v2) = 0;
			virtual void SetUniformFloat(const std::string& uniform, float v1, float v2, float v3) = 0;
			virtual void SetUniformFloat(const std::string& uniform, float v1, float v2, float v3, float v4) = 0;
			virtual void SetUniformFloat(const std::string& uniform, const Vector2& v) = 0;
			virtual void SetUniformFloat(const std::string& uniform, const Vector3& v) = 0;
			virtual void SetUniformFloat(const std::string& uniform, const Vector4& v) = 0;
			virtual void SetUniformFloat(const std::string& uniform, const std::vector<Vector2>& v) = 0;
			virtual void SetUniformFloat(const std::string& uniform, const std::vector<Vector3>& v) = 0;
			virtual void SetUniformFloat(const std::string& uniform, const std::vector<Vector4>& v) = 0;

			virtual void SetUniformInt(const std::string& uniform, int v1) = 0;
			virtual void SetUniformInt(const std::string& uniform, int v1, int v2) = 0;
			virtual void SetUniformInt(const std::string& uniform, int v1, int v2, int v3) = 0;
			virtual void SetUniformInt(const std::string& uniform, int v1, int v2, int v3, int v4) = 0;

			virtual void SetUniformMatrix(const std::string& uniform, const Matrix2& m) = 0;
			virtual void SetUniformMatrix(const std::string& uniform, const Matrix3& m) = 0;
			virtual void SetUniformMatrix(const std::string& uniform, const Matrix4& m) = 0;
			virtual void SetUniformMatrix(const std::string& uniform, const std::vector<Matrix2>& m) = 0;
			virtual void SetUniformMatrix(const std::string& uniform, const std::vector<Matrix3>& m) = 0;
			virtual void SetUniformMatrix(const std::string& uniform, const std::vector<Matrix4>& m) = 0;

			virtual int GetUniformLocation(const std::string& uniform) = 0;

			//virtual void Initialize() = 0;
		protected:

			std::string shaderFiles[(int)ShaderStages::SHADER_MAX];
		};
	}
}

