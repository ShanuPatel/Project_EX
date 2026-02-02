#include "PCH.h"
#include "RendererAPI.h"
#include "OpenGL/OpenGLRendererAPI.h"

namespace EX
{
	RendererAPI::API RendererAPI::sAPI = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (sAPI)
		{
		case RendererAPI::API::None: EX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!") return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
		}

		EX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;

	}
}