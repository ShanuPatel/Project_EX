#include "PCH.h"
#include "OpenGLShader.h"

#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include <regex>

namespace EX
{
	// ----------------------------
	// Shader type helpers
	// ----------------------------
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")   return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
		if (type == "geometry") return GL_GEOMETRY_SHADER;
		if (type == "compute")  return GL_COMPUTE_SHADER;

		EX_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	// ----------------------------
	// Factory
	// ----------------------------
	Ref<Shader> Shader::Create(const std::filesystem::path& filepath)
	{
		return CreateRef<OpenGLShader>(filepath);
	}

	// ----------------------------
	// Construction / Destruction
	// ----------------------------
	OpenGLShader::OpenGLShader(const std::filesystem::path& filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);

		mName = filepath.filename().string();
		EX_CORE_INFO("OpenGLShader: {0}", mName);

		Compile(shaderSources);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: mName(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(mRendererID);
	}

	// ----------------------------
	// File reading with includes
	// ----------------------------
	std::string OpenGLShader::ReadFile(const std::filesystem::path& filepath)
	{
		std::unordered_set<std::filesystem::path> includedFiles;
		std::unordered_map<std::filesystem::path, int> sourceIDMap;

		std::string source = ProcessShaderFile(filepath, includedFiles, sourceIDMap);

		for (const auto& [file, id] : sourceIDMap)
			mSourceIDToFile[id] = file;

		return source;
	}

	std::string OpenGLShader::ProcessShaderFile(
		const std::filesystem::path& filepath,
		std::unordered_set<std::filesystem::path>& includedFiles,
		std::unordered_map<std::filesystem::path, int>& sourceIDMap)
	{
		if (includedFiles.contains(filepath))
			return "";

		includedFiles.insert(filepath);

		if (mIncludeCache.contains(filepath))
			return mIncludeCache[filepath];

		int sourceID = sourceIDMap.contains(filepath)
			? sourceIDMap[filepath]
			: static_cast<int>(sourceIDMap.size());

		sourceIDMap[filepath] = sourceID;
		mSourceIDToFile[sourceID] = filepath;

		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (!in)
		{
			EX_CORE_ERROR("Could not open shader file '{0}'", filepath.string());
			return "";
		}

		std::string result;
		std::string line;
		size_t currentLine = 1;
		const std::filesystem::path currentDir = filepath.parent_path();

		result += "#line 1 " + std::to_string(sourceID) + "\n";

		while (std::getline(in, line))
		{
			if (line.find("#pragma once") != std::string::npos)
			{
				currentLine++;
				continue;
			}

			if (line.find("#include") != std::string::npos)
			{
				size_t firstQuote = line.find('"');
				size_t secondQuote = line.find('"', firstQuote + 1);

				if (firstQuote != std::string::npos && secondQuote != std::string::npos)
				{
					std::string includeFile = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
					std::filesystem::path includePath =
						std::filesystem::weakly_canonical(currentDir / includeFile);

					result += ProcessShaderFile(includePath, includedFiles, sourceIDMap);
					result += "#line " + std::to_string(currentLine + 1) + " " + std::to_string(sourceID) + "\n";
				}
				else
				{
					EX_CORE_ERROR("Invalid #include syntax in shader '{0}'", filepath.string());
				}
			}
			else
			{
				result += line + "\n";
			}

			currentLine++;
		}

		mIncludeCache[filepath] = result;
		return result;
	}

	// ----------------------------
	// Preprocess (#type blocks)
	// ----------------------------
	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;
		const std::string typeToken = "#type";
		size_t pos = 0;

		while ((pos = source.find(typeToken, pos)) != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			EX_CORE_ASSERT(eol != std::string::npos, "Syntax error after #type");

			size_t begin = pos + typeToken.size() + 1;
			std::string typeStr = source.substr(begin, eol - begin);
			GLenum type = ShaderTypeFromString(typeStr);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			size_t nextTypePos = source.find(typeToken, nextLinePos);

			shaderSources[type] = source.substr(nextLinePos, nextTypePos == std::string::npos ? std::string::npos : nextTypePos - nextLinePos);

			pos = nextTypePos;
		}

		return shaderSources;
	}

	// ----------------------------
	// Compilation & linking
	// ----------------------------
	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		EX_CORE_ASSERT(shaderSources.size() <= 3, "Only 3 shader stages supported");

		std::array<GLuint, 3> shaderIDs{};
		int index = 0;

		for (const auto& [type, source] : shaderSources)
		{
			GLuint shader = glCreateShader(type);
			const char* src = source.c_str();

			glShaderSource(shader, 1, &src, nullptr);
			glCompileShader(shader);

			GLint logLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
			if (logLength > 0)
			{
				std::vector<char> log(logLength);
				glGetShaderInfoLog(shader, logLength, nullptr, log.data());
				ParseAndLogShaderErrors(log.data());
			}

			GLint compiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			if (compiled == GL_FALSE)
			{
				glDeleteShader(shader);
				EX_CORE_ASSERT(false, "Shader compilation failed!");
				return;
			}

			glAttachShader(program, shader);
			shaderIDs[index++] = shader;
		}

		glLinkProgram(program);

		GLint linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (linked == GL_FALSE)
		{
			glDeleteProgram(program);
			for (GLuint id : shaderIDs)
				glDeleteShader(id);

			EX_CORE_ASSERT(false, "Shader linking failed!");
			return;
		}

		for (GLuint id : shaderIDs)
			glDetachShader(program, id);

		mRendererID = program;
	}

	// ----------------------------
	// Error parsing
	// ----------------------------
	void OpenGLShader::ParseAndLogShaderErrors(const char* log)
	{
		std::istringstream iss(log);
		std::string line;

		std::regex errorRegex(R"(ERROR:\s*(\d+):(\d+):\s*(.*))");
		std::regex warningRegex(R"(WARNING:\s*(\d+):(\d+):\s*(.*))");

		while (std::getline(iss, line))
		{
			std::smatch match;

			if (std::regex_match(line, match, errorRegex))
			{
				int sourceID = std::stoi(match[1]);
				int lineNum = std::stoi(match[2]);

				EX_CORE_ERROR("Shader error in '{0}' line {1}: {2}",
					GetFileFromSourceID(sourceID).string(), lineNum, match[3].str());
			}
			else if (std::regex_match(line, match, warningRegex))
			{
				int sourceID = std::stoi(match[1]);
				int lineNum = std::stoi(match[2]);

				EX_CORE_WARN("Shader warning in '{0}' line {1}: {2}", GetFileFromSourceID(sourceID).string(), lineNum, match[3].str());
			}
			else
			{
				EX_CORE_WARN("Shader log: {0}", line);
			}
		}
	}

	std::filesystem::path OpenGLShader::GetFileFromSourceID(int sourceID) const
	{
		auto it = mSourceIDToFile.find(sourceID);
		return it != mSourceIDToFile.end() ? it->second : "";
	}

	// ----------------------------
	// Binding
	// ----------------------------
	void OpenGLShader::Bind() const
	{
		glUseProgram(mRendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	// ----------------------------
	// Uniforms
	// ----------------------------
	int OpenGLShader::GetLocation(const std::string& name)
	{
		auto it = mUniformLocationCache.find(name);
		if (it != mUniformLocationCache.end())
			return it->second;

		int location = glGetUniformLocation(mRendererID, name.c_str());
		mUniformLocationCache[name] = location;
		return location;
	}

	void OpenGLShader::SetInt(const std::string& name, int value) { glUniform1i(GetLocation(name), value); }
	
	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		glUniform1iv(GetLocation(name), count, values);
	}

	void OpenGLShader::SetBool(const std::string& name, bool value) { glUniform1i(GetLocation(name), (int)value); }
	void OpenGLShader::SetFloat(const std::string& name, float value) { glUniform1f(GetLocation(name), value); }
	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& v) { glUniform2f(GetLocation(name), v.x, v.y); }
	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& v) { glUniform3f(GetLocation(name), v.x, v.y, v.z); }
	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& v) { glUniform4f(GetLocation(name), v.x, v.y, v.z, v.w); }
	
	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& m)
	{
		glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(m));
	}

	void OpenGLShader::SetUniformBlock(const std::string& name, uint32_t index)
	{
		glUniformBlockBinding(mRendererID, glGetUniformBlockIndex(mRendererID, name.c_str()), index);
	}
}