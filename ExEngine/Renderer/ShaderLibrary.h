#ifndef SHADER_LIBRARY
#define SHADER_LIBRARY

#include "Shader.h"

namespace EX
{
	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::filesystem::path& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filePath);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name)const;
	private:
		std::unordered_map < std::string, Ref<Shader>> mShaders;
	};
}
#endif // !SHADER_LIBRARY
