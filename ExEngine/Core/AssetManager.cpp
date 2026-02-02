#include "AssetManager.h"
#include "Application.h"
#include "Renderer/Texture.h"
#include "Renderer/Mesh.h"
#include <future>

#include <stb_image.h>

namespace EX
{
	//TODO Use filesystem in future
	void AssetManager::Init()
	{
		/* Init stuff here */
		SetFlipUV(false);
	}

	void AssetManager::Shutdown()
	{

		mTexture2DMap.clear();
		mTextureCubeMap.clear();
		mMeshMap.clear();
		mCubemapNames.clear(); // Reset names
	}

	static void LoadTexture2D(Texture2D* tex, const std::string_view path)
	{

		int width, height, channels;
		stbi_uc* data = nullptr;
		{
			data = stbi_load(path.data(), &width, &height, &channels, 0);
		}
		EX_CORE_ASSERT(data, "Failed to load image!")
	}

	static void LoadTextureCubemap(TextureCubemap* tex, const std::string_view path)
	{
		stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		float* data = nullptr;
		{
			data = stbi_loadf(path.data(), &width, &height, &channels, 0);
		}
		EX_CORE_ASSERT(data, "Failed to load image!")
	}// Store the cubemap names for UI

	void AssetManager::LoadAllIBLCubemaps(const std::string& directory)
	{
		for (const auto& entry : std::filesystem::directory_iterator(directory))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".hdr")
			{
				std::string path = entry.path().string();
				std::string name = entry.path().stem().string(); // e.g. "sky" from "sky.hdr"

				GetTextureCubemap(path);              // Load through the asset manager
				mCubemapNames.push_back(name);       // Store for UI display
			}
		}
	}

	void AssetManager::SetFlipUV(bool bFlipUV)
	{
		stbi_set_flip_vertically_on_load(bFlipUV);
	}

	Ref<Texture2D>& AssetManager::GetTexture2D(const std::string& path)
	{
		const auto& it = mTexture2DMap.find(path);
		if (it != mTexture2DMap.end())
			return it->second;

		Ref<Texture2D> texture = Texture2D::Create(path);
		mTexture2DMap.emplace(path, texture);
		return mTexture2DMap[path];
	}


	Ref<Mesh>& AssetManager::GetMesh(const std::string& path)
	{
		//auto absPath = std::filesystem::absolute(path);
		const auto& it = mMeshMap.find(path);
		if (it != mMeshMap.end())
			return it->second;

		Ref<Mesh> mesh = CreateRef<Mesh>(path.c_str());
		mMeshMap.emplace(path, mesh);
		return mMeshMap[path];
	}

	Ref<TextureCubemap>& AssetManager::GetTextureCubemap(const std::string& path)
	{

		const auto& it = mTextureCubeMap.find(path);
		if (it != mTextureCubeMap.end())
			return it->second;

		Ref<TextureCubemap> texture = TextureCubemap::Create(path);
		mTextureCubeMap.emplace(path, texture);
		return mTextureCubeMap[path];
	}

	const std::unordered_map<std::string, Ref<Texture2D>>& AssetManager::GetAllTextures2D()
	{
		return mTexture2DMap;
	}
}