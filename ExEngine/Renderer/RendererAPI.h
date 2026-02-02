#pragma once

#include <glm/glm.hpp>
#include "VertexArray.h"

enum class DepthFunc;
namespace EX
{
	class RendererAPI
	{
	public:
		enum class API
		{
			//add api like dx etc
			None = 0, OpenGL = 1
		};

	public:
		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void Draw(const Ref<VertexArray>& vertexArray, uint32_t count) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount = 0) = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0, uint32_t amount = 0) = 0;

		virtual void SetDepthTest(bool value) = 0;
		virtual void SetBlendState(bool value) = 0;
		virtual void SetDepthFunction(DepthFunc func) = 0;
		virtual void SetDepthMask(bool value) = 0;
		virtual void SetStencilTest(bool value) = 0;

		static Scope<RendererAPI> Create();
		inline static API GetAPI() { return sAPI; }

	private:
		static API sAPI;
	};
}