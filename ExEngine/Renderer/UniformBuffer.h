#pragma once

#include "Core/Core.h"
#include "Buffer.h"

namespace EX {

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset) = 0;
		virtual void SetLayout(const BufferLayout& layout, uint32_t blockIndex, uint32_t count = 1) = 0;
		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
		static Ref<UniformBuffer> Create();
	};

}