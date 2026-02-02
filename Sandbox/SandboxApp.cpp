#pragma once
#include "Engine.h"
#include "Core/EntryPoint.h"
#include "Sandbox.h"

namespace EX
{
	class SandboxApp : public Application
	{
	public:
		SandboxApp()
			:Application("Sandbox")
		{
			EX_CORE_INFO("Created Sandbox");
			mSandbox = CreateScope<Sandbox>();
			PushLayer(mSandbox.get());
		}
		~SandboxApp()
		{
			EX_CORE_INFO("Destroyed Sandbox");
			PopLayer(mSandbox.get());
		}
	private:
		Scope<Sandbox> mSandbox = nullptr;
	};

	Scope<Application> CreateApplication()
	{
		return CreateScope< SandboxApp>();
	}
}