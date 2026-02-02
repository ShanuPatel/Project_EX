#pragma once 

#include "Renderer/Texture.h"
#include "glad/glad.h"

namespace EX
{
	class OpenGLTexture2D :public Texture2D
	{
	public:
		OpenGLTexture2D() = default;
		OpenGLTexture2D(uint32_t width, uint32_t height);
		explicit OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(uint32_t width, uint32_t height, const std::vector<glm::vec3>& noiseData);
		~OpenGLTexture2D() override;

		OpenGLTexture2D(const OpenGLTexture2D& other) = default;
		OpenGLTexture2D(OpenGLTexture2D&& other) = default;

		virtual uint32_t GetWidth() const override { return mWidth; }
		virtual uint32_t GetHeight() const override { return mHeight; }
		virtual uint32_t GetRendererID() const override { return mRendererID; }
		void SetWidth(uint32_t width);
		void SetHeight(uint32_t height);
		void SetRendererID(uint64_t renderID);
		const std::string& GetPath() const override { return mPath; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;
		//virtual void UnBind(uint32_t slot = 0)const override;

		void Invalidate(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels) override;

		void WriteToFile(const std::string& filename);

	private:
		void InvalidateImpl(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels);
		void GenerateTextureAndMip(GLenum internalFormat, GLenum dataFormat, const void* data);
	private:
		std::string mPath;
		uint32_t mWidth = 0, mHeight = 0;
		uint32_t mRendererID = 0;
		GLenum mInternalFormat = 0, mDataFormat = 0;
	};
}