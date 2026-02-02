#ifndef LAYER_HEADER
#define LAYER_HEADER

#include "Core.h"
#include "Event/Event.h"
#include "DeltaTime.h"

namespace EX
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(DeltaTime timestep) {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName()const { return mDebugName; }
	private:
		std::string mDebugName;
	};
}

#endif // !LAYER_HEADER