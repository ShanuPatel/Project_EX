#include "PCH.h"
#include "OpenGLRendererAPI.h"
#include "Renderer/RenderCommand.h"
#include "glad/glad.h"

namespace EX
{
	void OpenGLRendererAPI::Init()
	{
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::Draw(const Ref<VertexArray>& vertexArray, uint32_t count)
	{
		vertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(count));
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, static_cast<int>(vertexCount));
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		const uint32_t count = indexCount != 0 ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, static_cast<int>(count), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t indexCount, uint32_t amount)
	{
		vertexArray->GetRendererID();
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, amount);
	}

	void OpenGLRendererAPI::SetDepthMask(bool value)
	{
		glDepthMask(value);
	}

	void OpenGLRendererAPI::SetStencilTest(bool value)
	{
		if (value)
		{
			//writing to stencil buffer
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

		}
		else
		{
			//disabling stencil writing 
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			SetDepthTest(value);

		}
	}

	void OpenGLRendererAPI::SetDepthTest(bool value)
	{
		if (value)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetBlendState(bool value)
	{
		if (value)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	void OpenGLRendererAPI::SetDepthFunction(DepthFunc func)
	{
		glDepthFunc(static_cast<GLenum>(func));
	}


}