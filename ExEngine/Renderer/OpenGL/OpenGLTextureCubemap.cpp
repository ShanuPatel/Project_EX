#include "PCH.h"
#include "OpenGLTextureCubemap.h"

#include "Renderer/Shader.h"
#include "Renderer/Renderer.h"
#include "OpenGLFramebuffer.h"

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace EX
{
	// ------------------------------------------------------------
	// Factory
	// ------------------------------------------------------------
	Ref<TextureCubemap> TextureCubemap::Create()
	{
		return CreateRef<OpenGLTextureCubemap>();
	}

	Ref<TextureCubemap> TextureCubemap::Create(const std::string& path)
	{
		return CreateRef<OpenGLTextureCubemap>(path);
	}

	// ------------------------------------------------------------
	// Construction / Destruction
	// ------------------------------------------------------------
	OpenGLTextureCubemap::OpenGLTextureCubemap(const std::string& path)
	{
		int width = 0, height = 0, channels = 0;
		stbi_set_flip_vertically_on_load(1);

		float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
		EX_CORE_ASSERT(data, "Failed to load HDR image!");

		InvalidateImpl(path, width, height, data, channels);
		stbi_image_free(data);
	}

	OpenGLTextureCubemap::~OpenGLTextureCubemap()
	{
		glDeleteTextures(1, &mHRDRendererID);
		glDeleteTextures(1, &mRendererID);
		glDeleteTextures(1, &mIrradianceRendererID);
		glDeleteTextures(1, &mRadianceRendererID);
		glDeleteTextures(1, &mBRDFLUTTextureID);
	}

	// ------------------------------------------------------------
	// Binding
	// ------------------------------------------------------------
	void OpenGLTextureCubemap::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, mRendererID);
	}

	void OpenGLTextureCubemap::BindIrradianceMap(uint32_t slot) const
	{
		glBindTextureUnit(slot, mIrradianceRendererID);
	}

	void OpenGLTextureCubemap::BindRadianceMap(uint32_t slot) const
	{
		glBindTextureUnit(slot, mRadianceRendererID);
	}

	void OpenGLTextureCubemap::BindBRDFLUT(uint32_t slot) const
	{
		glBindTextureUnit(slot, mBRDFLUTTextureID);
	}

	void OpenGLTextureCubemap::SetData(void* data, uint32_t size)
	{
		//TODO 
		//Used for DSA
	}

	// ------------------------------------------------------------
	// Invalidate
	// ------------------------------------------------------------
	void OpenGLTextureCubemap::Invalidate(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels)
	{
		InvalidateImpl(path, width, height, data, channels);
	}

	void OpenGLTextureCubemap::InvalidateImpl(std::string_view path, uint32_t width, uint32_t height, const void* data, uint32_t channels)
	{
		if (channels < 3)
		{
			EX_CORE_ERROR("Invalid HDR format ({} channels): {}", channels, path);
			return;
		}

		mPath = path;
		mWidth = width;
		mHeight = height;

		switch (channels)
		{
		case 1: mInternalFormat = GL_R16F;   mDataFormat = GL_RED;  break;
		case 2: mInternalFormat = GL_RG16F;  mDataFormat = GL_RG;   break;
		case 3: mInternalFormat = GL_RGB16F; mDataFormat = GL_RGB;  break;
		case 4: mInternalFormat = GL_RGBA16F; mDataFormat = GL_RGBA; break;
		default:
			EX_CORE_ERROR("Unsupported channel count: {}", channels);
			return;
		}

		CreateCubeMap(mRendererID, cubemapSize, mInternalFormat, mDataFormat);
		GenerateMipMap(true, true);

		CreateHdrTexture(mInternalFormat, mDataFormat, data);
		GenerateMipMap(false, true);
	}

	// ------------------------------------------------------------
	// Texture creation helpers
	// ------------------------------------------------------------
	void OpenGLTextureCubemap::CreateHdrTexture(GLenum internalFormat, GLenum dataFormat, const void* data)
	{
		glGenTextures(1, &mHRDRendererID);
		glBindTexture(GL_TEXTURE_2D, mHRDRendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, (int)mWidth, (int)mHeight, 0, dataFormat, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	void OpenGLTextureCubemap::CreateCubeMap(uint32_t& textureID, uint32_t size,GLenum internalFormat, GLenum dataFormat, GLenum type)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		for (uint32_t i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, size, size, 0, dataFormat, type, nullptr);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void OpenGLTextureCubemap::GenerateMipMap(bool cubeMap, bool generate)
	{
		GLenum target = cubeMap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;

		glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
			generate ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (generate)
			glGenerateMipmap(target);
	}

	void OpenGLTextureCubemap::CreateTexture2DEmpty(uint32_t width, uint32_t height)
	{
		glGenTextures(1, &mBRDFLUTTextureID);
		glBindTexture(GL_TEXTURE_2D, mBRDFLUTTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	// ------------------------------------------------------------
	// PBR generation pipeline
	// ------------------------------------------------------------
	void OpenGLTextureCubemap::GenerateCubeMap()
	{
		//Should be moved to thier own class should not be here i have not done in this sample project, one can easily move it to seperate class.
		EquirectangularToCubeMap(cubemapSize, mHRDRendererID, mRendererID);

		CreateCubeMap(mIrradianceRendererID, mIrradianceMapSize, GL_RGB16F, GL_RGB);
		GenerateMipMap(true);
		Irradiance(mIrradianceRendererID, mRendererID);

		CreateCubeMap(mRadianceRendererID, mPrefilterMapSize, GL_RGB16F, GL_RGB);
		GenerateMipMap(true, true);
		Prefilter(mRadianceRendererID, mRendererID);

		CreateTexture2DEmpty(mBRDFLUTSize, mBRDFLUTSize);
		GenerateMipMap(false);
		BRDF(mBRDFLUTTextureID);
	}

	// ------------------------------------------------------------
	// Rendering passes
	// ------------------------------------------------------------
	void OpenGLTextureCubemap::EquirectangularToCubeMap(uint32_t size, uint32_t hdrID, uint32_t outID)
	{
		if (!pbr.EquirectangularToCubemapShader)
			pbr.EquirectangularToCubemapShader =
			Shader::Create("bin/assets/shaders/EquirectangularToCubemap.glsl");

		FramebufferSpecification spec{ size, size, { FramebufferTextureFormat::CUBEMAP } };
		Ref<Framebuffer> fbo = Framebuffer::Create(spec);

		pbr.EquirectangularToCubemapShader->Bind();
		pbr.EquirectangularToCubemapShader->SetInt("uEquirectangularMap", 0);
		pbr.EquirectangularToCubemapShader->SetMat4("u_Projection", captureProjection);

		fbo->BindInputTexture(hdrID);
		RenderCommand::SetViewport(0, 0, size, size);

		for (uint32_t i = 0; i < 6; ++i)
		{
			pbr.EquirectangularToCubemapShader->SetMat4("u_View", captureViews[i]);
			fbo->BindFace(i, outID);
			RenderCommand::Clear();
			Renderer::DrawCube();
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, outID);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	void OpenGLTextureCubemap::Irradiance(uint32_t texture, uint32_t source)
	{
		if (!pbr.IrradianceShader)
			pbr.IrradianceShader = Shader::Create("bin/assets/shaders/Irradiance.glsl");

		FramebufferSpecification spec{ mIrradianceMapSize, mIrradianceMapSize, { FramebufferTextureFormat::CUBEMAP } };
		Ref<Framebuffer> fbo = Framebuffer::Create(spec);

		pbr.IrradianceShader->Bind();
		pbr.IrradianceShader->SetInt("u_EnvironmentMap", 0);
		pbr.IrradianceShader->SetMat4("u_Projection", captureProjection);

		fbo->BindInputTexture(source, 0, true);
		RenderCommand::SetViewport(0, 0, mIrradianceMapSize, mIrradianceMapSize);

		for (uint32_t i = 0; i < 6; ++i)
		{
			pbr.IrradianceShader->SetMat4("u_View", captureViews[i]);
			fbo->BindFace(i, texture);
			RenderCommand::Clear();
			Renderer::DrawCube();
		}
	}

	void OpenGLTextureCubemap::Prefilter(uint32_t texture, uint32_t source)
	{
		if (!pbr.RadianceShader)
			pbr.RadianceShader = Shader::Create("bin/assets/shaders/PrefilterCubemap.glsl");

		FramebufferSpecification spec{ mPrefilterMapSize, mPrefilterMapSize, { FramebufferTextureFormat::CUBEMAP } };
		Ref<Framebuffer> fbo = Framebuffer::Create(spec);

		pbr.RadianceShader->Bind();
		pbr.RadianceShader->SetInt("u_EnvironmentMap", 0);
		pbr.RadianceShader->SetMat4("u_Projection", captureProjection);

		fbo->BindInputTexture(source, 0, true);

		const uint32_t maxMipLevels =
			(uint32_t)glm::log2((float)mRadianceMapSize) + 1;

		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32_t mipSize = (uint32_t)(mPrefilterMapSize * std::pow(0.5f, mip));
			RenderCommand::SetViewport(0, 0, mipSize, mipSize);

			pbr.RadianceShader->SetFloat("u_Roughness", (float)mip / (float)(maxMipLevels - 1));

			for (uint32_t i = 0; i < 6; ++i)
			{
				pbr.RadianceShader->SetMat4("u_View", captureViews[i]);
				fbo->BindFace(i, texture, mip);
				RenderCommand::Clear();
				Renderer::DrawCube();
			}
		}
	}

	void OpenGLTextureCubemap::BRDF(uint32_t texture)
	{
		if (!pbr.BrdfShader)
			pbr.BrdfShader = Shader::Create("bin/assets/shaders/brdf.glsl");

		FramebufferSpecification spec{ mBRDFLUTSize, mBRDFLUTSize, { FramebufferTextureFormat::RG } };
		Ref<Framebuffer> fbo = Framebuffer::Create(spec);

		RenderCommand::SetViewport(0, 0, mBRDFLUTSize, mBRDFLUTSize);
		RenderCommand::Clear();

		pbr.BrdfShader->Bind();
		fbo->Bind2DTexture(texture);
		Renderer::DrawQuad();
	}
}