#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Renderer/Shader.h"

//Not Recommended for Production
typedef unsigned int GLenum;
typedef unsigned int GLuint;

namespace EX
{
	class OpenGLShader : public Shader
	{
		static const uint32_t MAX_POINT_LIGHTS = 3;
	public:
		OpenGLShader(const std::filesystem::path& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetBool(const std::string& name, bool value) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual void SetUniformBlock(const std::string& name, uint32_t blockIndex) override;

		virtual const std::string& GetName() const override { return mName; }

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void UploadUniformBool(const std::string& name, bool value);
		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		std::string ReadFile(const std::filesystem::path& filepath);
		std::string ProcessShaderFile(const std::filesystem::path& filepath, std::unordered_set<std::filesystem::path>& includedFiles, std::unordered_map<std::filesystem::path, int>& sourceIDMap);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		mutable std::unordered_map<std::filesystem::path, std::string> mIncludeCache;
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
		//void ShaderMaterialProperties(const GLuint program);
		virtual int GetLocation(const std::string& name)override;
		void ParseAndLogShaderErrors(const char* log);
		std::filesystem::path GetFileFromSourceID(int sourceID) const;
	public:

	private:
		uint32_t mRendererID;
		std::string mName;
		std::unordered_map<int, std::filesystem::path> mSourceIDToFile;
		std::unordered_set<std::string> mIncludedFiles;
		std::unordered_map<std::string, int> mUniformLocationCache;
		std::unordered_map<std::string, int> mUniformsLocations;
	};
}