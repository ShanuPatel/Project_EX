
#include "LayerStack.h"


EX::LayerStack::LayerStack()
{}

EX::LayerStack::~LayerStack()
{
	for (Layer* layer : mLayers)
		delete layer;
}

void EX::LayerStack::PushLayer(Layer* layer)
{


	mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
	mLayerInsertIndex++;
}

void EX::LayerStack::PushOverlay(Layer* overlay)
{
	mLayers.emplace_back(overlay);
	//overlay->OnAttach();
}

void EX::LayerStack::PopLayer(Layer* layer)
{
	auto it = std::find(mLayers.begin(), mLayers.end(), layer);
	if (it != mLayers.end())
	{
		mLayers.erase(it);
		mLayerInsertIndex--;
	}
}

void EX::LayerStack::PopOverlay(Layer* overlay)
{
	auto it = std::find(mLayers.begin(), mLayers.end(), overlay);
	if (it != mLayers.end())
		mLayers.erase(it);
}
