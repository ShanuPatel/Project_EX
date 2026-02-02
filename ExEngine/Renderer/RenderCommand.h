#pragma once

#include "RendererAPI.h"

enum class DepthFunc
{
	NEVER = 0x0200,
	LESS = 0x0201,
	EQUAL = 0x0202,
	LEQUAL = 0x0203,
	GREATER = 0x0204,
	NOTEQUAL = 0x0205,
	GEQUAL = 0x0206,
	ALWAYS = 0x0207
};

namespace EX
{
	class RenderCommand
	{
		inline static Scope<RendererAPI> sRendererAPI;
	public:
		inline static void SetAPI(Scope<RendererAPI> api)
		{
			sRendererAPI = std::move(api);
		}

		inline static void Init()
		{
			sRendererAPI->Init();
		}

		inline static RendererAPI& GetRendererAPI()
		{
			return *sRendererAPI;
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			sRendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			sRendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			sRendererAPI->Clear();
		}

		inline static void SetDepthMask(bool value)
		{
			sRendererAPI->SetDepthMask(value);
		}

		inline static void SetDepthFunction(DepthFunc func)
		{
			sRendererAPI->SetDepthFunction(func);
		}

		inline static void SetDepthTest(bool value)
		{
			sRendererAPI->SetDepthTest(value);
		}

		inline static void SetBlendState(bool value)
		{
			sRendererAPI->SetBlendState(value);
		}

		inline static void SetStencilTest(bool value)
		{
			sRendererAPI->SetStencilTest(value);
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			sRendererAPI->DrawIndexed(vertexArray, count);
		}

		inline static void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			sRendererAPI->DrawIndexedInstanced(vertexArray, count);
		}

		inline static void Draw(const Ref<VertexArray>& vertexArray, uint32_t count)
		{
			sRendererAPI->Draw(vertexArray, count);
		}

		inline static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount = 0)
		{
			sRendererAPI->DrawLines(vertexArray, vertexCount);
		}
	};

}