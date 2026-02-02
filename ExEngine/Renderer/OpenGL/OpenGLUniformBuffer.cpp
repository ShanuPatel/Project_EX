#include "PCH.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace EX 
{

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		return CreateRef<OpenGLUniformBuffer>(size, binding);
	}

	Ref<UniformBuffer> UniformBuffer::Create()
	{
		return CreateRef<OpenGLUniformBuffer>();
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer()
	{
		glCreateBuffers(1, &mRendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, mRendererID);
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &mRendererID);
		glNamedBufferData(mRendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, mRendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &mRendererID);
	}

	void OpenGLUniformBuffer::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mRendererID);
	}

	void OpenGLUniformBuffer::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	}

	void OpenGLUniformBuffer::SetLayout(const BufferLayout& layout, uint32_t blockIndex, uint32_t count)
	{
		mLayout = layout;

		uint32_t size = 0;
		for (const auto& element : mLayout)
			size += ShaderDataTypeSize(element.Type);

		size *= count;

		glBindBuffer(GL_UNIFORM_BUFFER, mRendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, blockIndex, mRendererID, 0, size);
	}

}