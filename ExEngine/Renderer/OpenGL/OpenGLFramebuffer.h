#pragma once 

#include "Renderer/Framebuffer.h"

namespace EX
{
	class OpenGLFramebuffer :public Framebuffer
	{
	public:
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Bind2DTexture(uint32_t textureID) override;
		virtual void BindFace(uint32_t faceIndex, uint32_t textueID, int mipLevel = 0) override;
		virtual void BindInputTexture(uint32_t textureID, uint32_t unitIndex = 0, bool IsCubeMap = false) override;
		virtual void Unbind() override;

		virtual uint32_t GetWidth()const override;
		virtual uint32_t GetHeight()const override;
		virtual void Resize(uint32_t width, uint32_t height) override;
		//virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		// index = Attachments index, slot = texture slot ie Texture2D
		void BindColorAttachment(uint32_t index, uint32_t slot) override;
		void BindDepthAttachment(uint32_t slot) override;

		//virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint64_t GetColorAttachmentRendererID(uint32_t index = 0) const override;
		virtual uint64_t GetDepthAttachmentRendererID() const override { return mDepthAttachment; }
		virtual const FramebufferSpecification& GetSpecification() const override { return mSpecification; }
	private:
		uint32_t mRendererID = 0;
		uint32_t mCubeMapID = 0;
		//uint32_t mColorAttachment = 0;
		FramebufferSpecification mSpecification;

		std::vector<FramebufferTextureSpecification> mColorAttachmentSpecifications;
		FramebufferTextureSpecification mDepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> mColorAttachments;
		uint32_t mDepthAttachment = 0;
	};
}