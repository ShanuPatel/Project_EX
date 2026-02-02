#pragma once
#include "Core.h"
#include "Application.h"

//Uncomment if using on laptop
//#if _WIN32
//#define EXPORT_SYMBOL __declspec(dllexport)
//#else
//#define EXPORT_SYMBOL
//#endif
//
//// Enable usage of more performant GPUs on laptops.
//extern "C" {
//	EXPORT_SYMBOL int NvOptimusEnablement = 1;
//	EXPORT_SYMBOL int AmdPowerXpressRequestHighPerformance = 1;
//}

extern Scope<EX::Application> CreateApplication();

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	 //break on a specific allocation if needed
	//_CrtSetBreakAlloc(342);
	EX::Log::Init();
	{
		auto App = EX::CreateApplication();
		App->Run();
	}
	EX::Log::Shutdown();

	return 0;
}