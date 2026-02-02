#pragma once

#include "Renderer/UniformBuffer.h"

namespace EX {

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer();
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer()override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;
		virtual void SetLayout(const BufferLayout& layout, uint32_t blockIndex, uint32_t count = 1) override;
	private:
		uint32_t mRendererID = 0;
		BufferLayout mLayout;
	};
}