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

namespace NCL {
	namespace Rendering {
		/**
		 * @brief OpenGL implementation of a shader.
		 */
		class OGLShader : public ShaderBase {
		public:
			/**
			 * @brief Create new shader program with the given source files.
			 * @brief If one Tesselation shader is specified, both must be.
			 * 
			 * @param vert Asset filename for the Vertex Shader.
			 * @param frag Asset filename for the Fragment Shader.
			 * @param tesc Asset filename for the Tesselation Control (Hull)
			 * Shader.
			 * @param tese Asset filename for the Tesselation Evaluation
			 * (Domain) Shader.
			 * @param geom Asset filename for the Geometry Shader.
			 */
			OGLShader(const std::string& vert, const std::string& frag, const std::string& tesc = "", const std::string& tese = "", const std::string& geom = "");
			~OGLShader() override;

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

			/**
			 * @return true if the program has loaded and compiled
			 * successfully, otherwise false.
			 */
			bool LoadSuccess() const {
				return programValid == GL_TRUE;
			}

			/**
			 * @return OpenGL program ID for this shader.
			 */
			int GetProgramID() const {
				return programID;
			}

			/**
			 * @brief Create a new shader from the given asset files.
			 * @brief See OGLShader::OGLShader().
			 */
			static std::unique_ptr<ShaderBase> CreateShader(const std::string& vertex, const std::string& fragment, const std::string& tessCont, const std::string& tessEval, const std::string& geometry);
			/**
			 * @brief Create a new shader from the given asset files and call Initialize.
			 * @brief See OGLShader::CreateShader().
			 */
			static std::unique_ptr<ShaderBase> CreateShaderAndInit(const std::string& vertex, const std::string& fragment, const std::string& tessCont, const std::string& tessEval, const std::string& geometry);

			static void PrintCompileLog(GLuint object);
			static void PrintLinkLog(GLuint program);
		protected:
			void LoadPass(const GLchar* code, ShaderStage type);
			void Clear();
			void DeleteIDs();

			GLuint programID = 0;
			GLint shaderValid[(size_t)ShaderStage::Max]{};
			GLint programValid = GL_FALSE;

			std::unordered_map<std::string, GLint> uniformMap{};
		};
	}
}
