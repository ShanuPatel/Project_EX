#pragma once

#include "Core/Core.h"

namespace EX
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RGBA16F,
		RGBA32F,
		R11G11B10,
		RG16F,
		RG,
		R32I,
		RED,

		// Depth/Stencil
		DEPTH24STENCIL8,
		CUBEMAP,
		CUBEMAPDEPTH,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {
		}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {
		}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Bind2DTexture(uint32_t textureID) = 0;
		virtual void BindFace(uint32_t faceIndex, uint32_t textueID, int mipLevel = 0) = 0;
		virtual void BindInputTexture(uint32_t textureID, uint32_t unitIndex = 0, bool IsCubeMap = false) = 0;
		virtual void Unbind() = 0;

		virtual void BindColorAttachment(uint32_t index, uint32_t slot) = 0;
		virtual void BindDepthAttachment(uint32_t slot) = 0;

		virtual uint32_t GetWidth()const = 0;
		virtual uint32_t GetHeight()const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		//virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual uint64_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint64_t GetDepthAttachmentRendererID() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}