#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace EX
{
	enum class MaterialPropertyType
	{
		None = 0,
		Sampler2D,
		Bool,
		Int,
		Float,
		Float2,
		Float3,
		Float4,
	};

	static size_t GetSizeInBytes(MaterialPropertyType type)
	{
		switch (type)
		{
		case MaterialPropertyType::None: return 0;
		case MaterialPropertyType::Sampler2D: return sizeof(int32_t);
		case MaterialPropertyType::Bool: return sizeof(int32_t);
		case MaterialPropertyType::Int: return sizeof(int32_t);
		case MaterialPropertyType::Float: return sizeof(float);
		case MaterialPropertyType::Float2: return sizeof(glm::vec2);
		case MaterialPropertyType::Float3: return sizeof(glm::vec3);
		case MaterialPropertyType::Float4: return sizeof(glm::vec4);
		default:
			//GLSL_CORE_ASSERT("Invalid MaterialPropertyType value");
			std::cout << "";
		}
		return 0;
	}

	struct MaterialProperty
	{
		MaterialPropertyType Type;
		size_t SizeInBytes;
		size_t OffsetInBytes;
		std::string DisplayName;
		bool IsSlider;
		bool IsColor;
		int SortOrder = 999;
	};

	class Shader {
	public:
		virtual ~Shader() = default;

		virtual void Bind()const = 0;
		virtual void Unbind()const = 0;


		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetBool(const std::string& name, bool value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;

		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual void SetUniformBlock(const std::string& name, uint32_t blockIndex) = 0;

		virtual const std::string& GetName()const = 0;
		virtual int GetLocation(const std::string& name) = 0;
		static Ref<Shader> Create(const std::filesystem::path& filepath);

	};

}