#pragma once

#include "Renderer/Texture.h"
#include <glad/glad.h>

namespace EX
{
	class Shader;
	class OpenGLTextureCubemap : public TextureCubemap
	{
	public:
		OpenGLTextureCubemap() = default;
		explicit OpenGLTextureCubemap(const std::string& path);
		//void CreateHdrCubeMap(void* data);
		~OpenGLTextureCubemap() override;

		OpenGLTextureCubemap(const OpenGLTextureCubemap& other) = default;
		OpenGLTextureCubemap(OpenGLTextureCubemap&& other) = default;

		uint32_t GetWidth() const override { return mWidth; }
		uint32_t GetHeight() const override { return mHeight; }
		uint32_t GetRendererID() const override { return mRendererID; }
		uint32_t GetHRDRendererID() const override { return mHRDRendererID; }
		uint32_t GetIrradianceMapID() const { return mIrradianceRendererID; }
		uint32_t GetRadianceMapID() const { return mRadianceRendererID; }
		uint32_t GetBRDFLUTRendererID()const override { return mBRDFLUTTextureID; }
		uint32_t GetCubemapSize()const { return cubemapSize; }
		const std::string& GetPath() const override { return mPath; }

		void SetHRDRendererID(uint32_t renderId) { mHRDRendererID = renderId; }
		void SetIrradianceMapID(uint32_t renderId) { mIrradianceRendererID = renderId; }
		void SetRadianceMapID(uint32_t renderId) { mRadianceRendererID = renderId; }
		void SetBRDFLUTRendererID(uint32_t renderId) { mBRDFLUTTextureID = renderId; }

		//needed to justify interface
		void SetData(void* data, uint32_t size) override;

		void Invalidate(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels) override;

		void Bind(uint32_t slot = 0) const override;
		void BindIrradianceMap(uint32_t slot) const override;
		void BindRadianceMap(uint32_t slot) const override;
		void BindBRDFLUT(uint32_t slot) const override;

		void InvalidateImpl(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels);
		void CreateHdrTexture(GLenum internalFormat, GLenum dataFormat, const void* data);
		void CreateCubeMap(uint32_t& textureID, uint32_t size, GLenum internalFormat, GLenum dataFormat, GLenum type = GL_FLOAT);
		void GenerateMipMap(bool bCubeMap = false, bool bGenerateMipmaps = false);
		void CreateTexture2DEmpty(uint32_t width, uint32_t height);
		void GenerateCubeMap() override;

	private:
		void EquirectangularToCubeMap(uint32_t cubemapSize, uint32_t hdrTextureID, uint32_t renderID);
		void Irradiance(uint32_t texture, uint32_t renderID);
		void Prefilter(uint32_t texture, uint32_t renderID);
		void BRDF(uint32_t texture);

	private:
		std::string mPath;
		uint32_t mWidth = 0, mHeight = 0;
		uint32_t mRendererID = 0;
		GLenum mInternalFormat = 0, mDataFormat = 0;


		//HDR
		uint32_t mHRDRendererID = 0;
		//IBL
		uint32_t mIrradianceRendererID = 0;
		uint32_t mRadianceRendererID = 0;
		uint32_t mBRDFLUTTextureID = 0;

		//PBR
		struct Shaders
		{
			Ref<Shader> EquirectangularToCubemapCompute;
			Ref<Shader> EquirectangularToCubemapShader;
			Ref<Shader> IrradianceShader;
			Ref<Shader> RadianceShader;
			Ref<Shader> BrdfShader;
		}pbr;
		const uint32_t cubemapSize = 1024;
		const uint32_t mIrradianceMapSize = 32;
		const uint32_t mRadianceMapSize = 128; //cubemapSize / 4; // for dedicated card prefilter and this is same
		const uint32_t mPrefilterMapSize = 128;//for integrated cards
		const uint32_t mBRDFLUTSize = 256;

		const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		const glm::mat4 captureViews[18] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
	};
}