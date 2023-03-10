/**
 * @file   OGLShader.h
 * @brief  OpenGL implementation of a shader.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "ShaderBase.h"
#include "glad\gl.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace NCL::Rendering {
	/**
	 * @brief OpenGL implementation of a shader.
	 */
	class OGLShader : public ShaderBase {
	public:
		OGLShader(const std::string& vert, const std::string& frag, const std::string& tesc = "", const std::string& tese = "", const std::string& geom = "");
		~OGLShader();

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

		bool LoadSuccess() const {
			return programValid == GL_TRUE;
		}

		int GetProgramID() const {
			return programID;
		}

		static std::unique_ptr<ShaderBase> CreateShader(const std::string& vertex, const std::string& fragment);
		static std::unique_ptr<ShaderBase> CreateShaderAndInit(const std::string& vertex, const std::string& fragment);

		static void PrintCompileLog(GLuint object);
		static void PrintLinkLog(GLuint program);

		void Initilize();
	protected:
		void LoadPass(const GLchar* code, ShaderStage type);
		void Clear();
		void DeleteIDs();

		GLuint programID;
		GLint shaderValid[(size_t)ShaderStage::Max];
		GLint programValid;

		std::unordered_map<std::string, GLint> uniformMap{};
	};
}
