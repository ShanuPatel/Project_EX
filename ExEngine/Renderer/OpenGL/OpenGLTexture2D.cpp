#include "PCH.h"
#include "OpenGLTexture2D.h"

#include <stb_image.h>
#include <stb_image_write.h>

namespace EX
{

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		return CreateRef<OpenGLTexture2D>(path);
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		return CreateRef<OpenGLTexture2D>(width, height);
	}


	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: mWidth(width), mHeight(height)
	{
		mInternalFormat = GL_RGBA8;
		mDataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
		glTextureStorage2D(mRendererID, 1, mInternalFormat, static_cast<int>(mWidth), static_cast<int>(mHeight));

		glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
	{
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		stbi_uc* data = nullptr;
		{
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		EX_CORE_ASSERT(data, "Failed to load image!");

		InvalidateImpl(path, width, height, data, channels);

		stbi_image_free(data);

	}

	void OpenGLTexture2D::InvalidateImpl(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels)
	{
		mPath = path;

		if (mRendererID)
			glDeleteTextures(1, &mRendererID);

		mWidth = width;
		mHeight = height;

		GLenum internalFormat = 0, dataFormat = 0;
		switch (channels)
		{
		case 1:
			internalFormat = GL_R8;
			dataFormat = GL_RED;
			break;
		case 2:
			internalFormat = GL_RG8;
			dataFormat = GL_RG;
			break;
		case 3:
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
			break;
		case 4:
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
			break;
		default:
			EX_CORE_ERROR("Texture channel count is not within (1-4) range. Channel count: {}", channels);
			break;
		}

		mInternalFormat = internalFormat;
		mDataFormat = dataFormat;

		EX_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		GenerateTextureAndMip(mInternalFormat, mDataFormat, data);
	}

	void OpenGLTexture2D::GenerateTextureAndMip(GLenum internalFormat, GLenum dataFormat, const void* data)
	{
		glGenTextures(1, &mRendererID);
		glBindTexture(GL_TEXTURE_2D, mRendererID);

		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(internalFormat), static_cast<int>(mWidth), static_cast<int>(mHeight), 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const std::vector<glm::vec3>& noiseData)
		: mWidth(width), mHeight(height)
	{
		mInternalFormat = GL_RGB32F;
		mDataFormat = GL_RGB;

		glGenTextures(1, &mRendererID);
		glBindTexture(GL_TEXTURE_2D, mRendererID);

		glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, width, height, 0, mDataFormat, GL_FLOAT, noiseData.data());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &mRendererID);
	}

	void OpenGLTexture2D::SetWidth(uint32_t width)
	{
		mWidth = width;
	}

	void OpenGLTexture2D::SetHeight(uint32_t height)
	{
		mHeight = height;
	}

	void OpenGLTexture2D::SetRendererID(uint64_t renderID)
	{
		mRendererID = renderID;
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		EX_CORE_ASSERT(size == mWidth * mHeight * (mDataFormat == GL_RGBA ? 4 : 3), "Data must be entire texture!")
			glTextureSubImage2D(mRendererID, 0, 0, 0, static_cast<int>(mWidth), static_cast<int>(mHeight), mDataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Invalidate(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels)
	{
		InvalidateImpl(path, width, height, data, channels);
	}

	//void OpenGLTexture2D::WriteToFile(const std::string& filename)
	//{
	//	const int width = 512;
	//	const int height = 512;
	//	// Allocate memory to read back texture data (GL_RG)
	//	std::vector<float> textureData(width * height * 2); // GL_RG has 2 components per pixel
	//	glReadPixels(0, 0, width, height, GL_RG, GL_FLOAT, textureData.data());

	//	// Convert GL_RG to GL_RGB for stb_image_write
	//	std::vector<unsigned char> rgbData(width * height * 3);
	//	for (int i = 0; i < width * height; ++i)
	//	{
	//		float r = textureData[i * 2];     // Red component
	//		float g = textureData[i * 2 + 1]; // Green component
	//		unsigned char rByte = static_cast<unsigned char>(r * 255.0f);
	//		unsigned char gByte = static_cast<unsigned char>(g * 255.0f);

	//		// Store as RGB (set B to 0)
	//		rgbData[i * 3] = rByte;
	//		rgbData[i * 3 + 1] = gByte;
	//		rgbData[i * 3 + 2] = 0;
	//	}

	//	// Write to file
	//	//const char* filepath = "E:\\GameEngine\\Project_GLSL\\GLEditor\\assets\\Textures\\brdf_texture.png";
	//	 // Base directory path
	//	const std::string directory = "E:\\GameEngine\\Project_GLSL\\GLEditor\\assets\\Textures\\";

	//	// Full file path by appending the filename with extension
	//	const std::string filepath = directory + filename + ".png";
	//	int reuslt = stbi_write_png(filepath.c_str(), width, height, 3, rgbData.data(), width * 3);
	//}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, mRendererID);
	}


}