#include "PCH.h"
#include "ShaderLibrary.h"

namespace EX
{
	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		EX_CORE_ASSERT(!Exists(name), "shader already exists!");
		mShaders[name] = shader;
	}
	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::filesystem::path& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;

	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filePath)
	{
		auto shader = Shader::Create(filePath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		EX_CORE_ASSERT(Exists(name), "shader not found!");
		return mShaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return mShaders.find(name) != mShaders.end();
	}
}