#pragma once

#include "Renderer/RendererAPI.h"

namespace EX
{
	class  OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		void Draw(const Ref<VertexArray>& vertexArray, uint32_t count) override;
		void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount = 0) override;
		void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0, uint32_t amount = 0) override;

		void SetDepthTest(bool value) override;
		void SetBlendState(bool value) override;
		void SetDepthFunction(DepthFunc func) override;
		void SetDepthMask(bool value) override;
		virtual void SetStencilTest(bool value)override;
	};
}