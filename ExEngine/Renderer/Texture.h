#pragma once

#include "Core/Core.h"
#include <string>

namespace EX
{
	class Texture 
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight()const = 0;
		virtual uint32_t GetRendererID()const = 0;
		virtual const std::string& GetPath() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		
		virtual void Bind(uint32_t slot = 0)const = 0;
		//virtual void UnBind(uint32_t slot = 0)const = 0;

		virtual void Invalidate(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels) = 0;

		bool operator==(const Texture& other) const { return GetRendererID() == other.GetRendererID(); }
	};

	class Texture2D :public Texture
	{
	public:
		//static Ref<Texture2D> Create();
		static Ref<Texture2D> Create(const std::string& path);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		//Create Float Noise Texture//ssao
		//static Ref<Texture2D> Create(uint32_t width, uint32_t height, const std::vector<glm::vec3>& noiseData);
	};

	class TextureCubemap : public Texture
	{
	public:
		static Ref<TextureCubemap> Create();
		static Ref<TextureCubemap> Create(const std::string& path);

		virtual void BindIrradianceMap(uint32_t slot) const = 0;
		virtual void BindRadianceMap(uint32_t slot) const = 0;
		virtual void BindBRDFLUT(uint32_t slot) const = 0;
		virtual uint32_t GetHRDRendererID() const = 0;
		virtual uint32_t GetBRDFLUTRendererID() const = 0;
		virtual uint32_t GetIrradianceMapID() const = 0;
		virtual uint32_t GetRadianceMapID() const = 0;
		virtual void GenerateCubeMap() = 0;
	};
}