#include "PCH.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace EX
{
	Ref<VertexArray> VertexArray::Create()
	{
		return CreateRef<OpenGLVertexArray>();
	}

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
		}

		EX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &mRendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &mRendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(mRendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		EX_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(mRendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(mVertexBufferIndex);
				glVertexAttribPointer(mVertexBufferIndex, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE,
										 static_cast<int>(layout.GetStride()), reinterpret_cast<const void*>(element.Offset));
				mVertexBufferIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				const uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(mVertexBufferIndex);
					glVertexAttribPointer(mVertexBufferIndex, count, ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE, 
											static_cast<int>(layout.GetStride()), reinterpret_cast<const void*>(element.Offset + sizeof(float) * count * i));
					glVertexAttribDivisor(mVertexBufferIndex, 1);
					mVertexBufferIndex++;
				}
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			{
				glEnableVertexAttribArray(mVertexBufferIndex);
				glVertexAttribIPointer(mVertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					static_cast<int>(layout.GetStride()),
					reinterpret_cast<const void*>(element.Offset));
				mVertexBufferIndex++;
				break;
			}
			default:
				EX_CORE_ASSERT(false, "Unknown ShaderDataType!")
					break;
			}
		}
		mVertexBuffers.emplace_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(mRendererID);
		indexBuffer->Bind();

		mIndexBuffer = indexBuffer;
	}
}