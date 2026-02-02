#include "PCH.h"
#include "OpenGLFramebuffer.h"
#include <glad/glad.h>

namespace EX
{
	static constexpr uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled, bool isCubeMap = false)
		{
			if (isCubeMap)
				return GL_TEXTURE_CUBE_MAP;
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void SetTextureParams(GLenum target, bool linear = true, bool isDepth = false)
		{
			GLenum wrap = isDepth ? GL_CLAMP_TO_BORDER : GL_CLAMP_TO_EDGE;
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
			glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
			if (target != GL_TEXTURE_CUBE_MAP)
				glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count, bool isCubeMap = false)
		{
			glCreateTextures(TextureTarget(multisampled, isCubeMap), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id, bool isCubeMap = false)
		{
			glBindTexture(TextureTarget(multisampled, isCubeMap), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index, GLenum type = GL_UNSIGNED_BYTE)
		{
			bool multisampled = samples > 1;
			GLenum target = TextureTarget(multisampled);

			if (multisampled)
			{
				glTexImage2DMultisample(target, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, nullptr);
				SetTextureParams(target);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, target, id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum internalFormat, GLenum attachment, uint32_t width, uint32_t height, bool isCube = false)
		{
			bool multisampled = samples > 1;
			GLenum target = TextureTarget(multisampled, isCube);

			if (multisampled)
			{
				glTexImage2DMultisample(target, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(target, 1, internalFormat, width, height);
				SetTextureParams(target, false, true);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, id, 0);
		}

		static void AttachCubeMapDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachment, uint32_t width, uint32_t height)
		{
			if (samples > 1)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				for (uint32_t i = 0; i < 6; ++i)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_FLOAT, nullptr);

				SetTextureParams(GL_TEXTURE_CUBE_MAP, false, true);
			}

			glFramebufferTexture(GL_FRAMEBUFFER, attachment, id, 0);
		}

		static void CreateCubeMapTexture(uint32_t* outID, uint32_t size, GLenum internalFormat, GLenum format, GLenum type, bool generateMipmaps = false)
		{
			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, outID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, *outID);

			for (uint32_t i = 0; i < 6; ++i)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, size, size, 0, format, type, nullptr);

			SetTextureParams(GL_TEXTURE_CUBE_MAP, generateMipmaps);
			if (generateMipmaps)
				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			return format == FramebufferTextureFormat::DEPTH24STENCIL8 || format == FramebufferTextureFormat::CUBEMAPDEPTH;
		}
	}

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		return CreateRef<OpenGLFramebuffer>(spec);
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: mSpecification(spec)
	{


		for (const auto& format : mSpecification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(format.TextureFormat))
				mColorAttachmentSpecifications.emplace_back(format);
			else
				mDepthAttachmentSpecification = format;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{


		glDeleteFramebuffers(1, &mRendererID);
		glDeleteTextures(static_cast<int>(mColorAttachments.size()), mColorAttachments.data());
		glDeleteTextures(1, &mDepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{


		if (mRendererID)
		{
			glDeleteFramebuffers(1, &mRendererID);
			glDeleteTextures((GLsizei)mColorAttachments.size(), mColorAttachments.data());
			glDeleteTextures(1, &mDepthAttachment);
			mColorAttachments.clear();
			mDepthAttachment = 0;
		}

		glCreateFramebuffers(1, &mRendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);

		bool multisample = mSpecification.Samples > 1;

		// Color attachments
		if (!mColorAttachmentSpecifications.empty())
		{
			mColorAttachments.resize(mColorAttachmentSpecifications.size());
			Utils::CreateTextures(multisample, mColorAttachments.data(), (uint32_t)mColorAttachments.size());

			for (size_t i = 0; i < mColorAttachmentSpecifications.size(); ++i)
			{
				auto& spec = mColorAttachmentSpecifications[i];
				uint32_t texID = mColorAttachments[i];
				Utils::BindTexture(multisample, texID);

				switch (spec.TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(texID, mSpecification.Samples, GL_RGBA8, GL_RGBA, mSpecification.Width, mSpecification.Height, (int)i);
					break;
				case FramebufferTextureFormat::RGBA16F:
					Utils::AttachColorTexture(texID, mSpecification.Samples, GL_RGBA16F, GL_RGBA, mSpecification.Width, mSpecification.Height, (int)i);
					break;
				case FramebufferTextureFormat::RGBA32F:
					Utils::AttachColorTexture(texID, mSpecification.Samples, GL_RGBA32F, GL_RGBA, mSpecification.Width, mSpecification.Height, (int)i);
					break;
				case FramebufferTextureFormat::R11G11B10:
					Utils::AttachColorTexture(texID, mSpecification.Samples, GL_R11F_G11F_B10F, GL_RGB, mSpecification.Width, mSpecification.Height, (int)i);
					break;
				case FramebufferTextureFormat::RG16F:
					Utils::AttachColorTexture(texID, mSpecification.Samples, GL_RG16F, GL_RG, mSpecification.Width, mSpecification.Height, (int)i);
					break;
				case FramebufferTextureFormat::R32I:
					Utils::AttachColorTexture(texID, mSpecification.Samples, GL_R32I, GL_RED_INTEGER, mSpecification.Width, mSpecification.Height, (int)i);
					break;
				case FramebufferTextureFormat::RED:
					Utils::AttachColorTexture(texID, mSpecification.Samples, GL_R32F, GL_RED, mSpecification.Width, mSpecification.Height, (int)i, GL_FLOAT);
					break;
				case FramebufferTextureFormat::CUBEMAP:
					// Will be attached externally later in IBL pipeline
					break;
				default:
					if(spec.TextureFormat != FramebufferTextureFormat::RG)
					EX_CORE_ASSERT(false, "Unknown color attachment format");
					break;
				}
			}
		}

		// Depth attachment
		if (mDepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			bool isCube = mDepthAttachmentSpecification.TextureFormat == FramebufferTextureFormat::CUBEMAPDEPTH;
			Utils::CreateTextures(multisample, &mDepthAttachment, 1, isCube);
			Utils::BindTexture(multisample, mDepthAttachment, isCube);

			switch (mDepthAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				Utils::AttachDepthTexture(mDepthAttachment, mSpecification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, mSpecification.Width, mSpecification.Height);
				break;
			case FramebufferTextureFormat::CUBEMAPDEPTH:
				Utils::AttachCubeMapDepthTexture(mDepthAttachment, mSpecification.Samples, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, mSpecification.Width, mSpecification.Height);
				break;
			default:
				EX_CORE_ASSERT(false, "Unknown depth attachment format");
				break;
			}
		}

		// Final setup
		if (mColorAttachments.size() > 1)
		{
			GLenum buffers[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
			EX_CORE_ASSERT(mColorAttachments.size() <= 5);
			glDrawBuffers((GLsizei)mColorAttachments.size(), buffers);
		}
		else if (mColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		const auto& format = mSpecification.Attachments.Attachments[0].TextureFormat;

		if (format != FramebufferTextureFormat::CUBEMAP && format != FramebufferTextureFormat::RG)
		{
			EX_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		}
		else
		{
			auto FramebufferTextureFormatToString = [](FramebufferTextureFormat format) -> std::string
				{
					switch (format)
					{
					case FramebufferTextureFormat::RGBA8: return "RGBA8";
					case FramebufferTextureFormat::RG: return "RG";
					//case FramebufferTextureFormat::RED_INTEGER: return "RED_INTEGER";
					case FramebufferTextureFormat::DEPTH24STENCIL8: return "DEPTH24STENCIL8";
					case FramebufferTextureFormat::CUBEMAP: return "CUBEMAP";

					default: return "Unknown";
					}
				};
			EX_CORE_WARN("Skipping framebuffer completeness check for format: {}", FramebufferTextureFormatToString(format));
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
		glViewport(0, 0, static_cast<int>(mSpecification.Width), static_cast<int>(mSpecification.Height));
	}

	void OpenGLFramebuffer::Bind2DTexture(uint32_t textureID)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
		glViewport(0, 0, mSpecification.Width, mSpecification.Height);

		EX_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
	}

	void OpenGLFramebuffer::BindFace(uint32_t faceIndex, uint32_t textueID, int mipLevel)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, textueID, mipLevel);
		if(!mipLevel)
		glViewport(0, 0, mSpecification.Width, mSpecification.Height);

		EX_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
	}

	void OpenGLFramebuffer::BindInputTexture(uint32_t textureID, uint32_t unitIndex, bool IsCubeMap)
	{
		if(IsCubeMap)
		{
			glActiveTexture(GL_TEXTURE0 + unitIndex);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0 + unitIndex);       // convert unit index to GLenum
			glBindTexture(GL_TEXTURE_2D, textureID);         // bind texture
		}
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	uint32_t OpenGLFramebuffer::GetWidth() const
	{
		return mSpecification.Width;
	}

	uint32_t OpenGLFramebuffer::GetHeight() const
	{
		return mSpecification.Height;
	}

	void OpenGLFramebuffer::BindColorAttachment(uint32_t index, uint32_t slot)
	{
		EX_CORE_ASSERT(index < mColorAttachments.size(), "");
		glBindTextureUnit(slot, mColorAttachments[index]);
	}

	void OpenGLFramebuffer::BindDepthAttachment(uint32_t slot)
	{
		glBindTextureUnit(slot, mDepthAttachment);
	}

	uint64_t OpenGLFramebuffer::GetColorAttachmentRendererID(uint32_t index) const
	{
		EX_CORE_ASSERT(index < mColorAttachments.size());
		return mColorAttachments[index];
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			EX_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}

		mSpecification.Width = width;
		mSpecification.Height = height;

		Invalidate();
	}

	//void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	//{
	//}

	//int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	//{
	//	EX_CORE_ASSERT(attachmentIndex < mColorAttachments.size());
	//	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
	//	int pixelData;
	//	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);// temp hardcoded need modification
	//	return pixelData;
	//}
}