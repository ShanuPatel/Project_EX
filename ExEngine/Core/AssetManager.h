#pragma once

namespace EX {
	class Mesh;
	class TextureCubemap;
	class Texture2D;
	class AssetManager
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetFlipUV(bool bFlipUV = true);
		static Ref<Texture2D>& GetTexture2D(const std::string& path);
		static const std::unordered_map<std::string, Ref<Texture2D>>& GetAllTextures2D();
		static Ref<TextureCubemap>& GetTextureCubemap(const std::string& path);
		static Ref<Mesh>& GetMesh(const std::string& path);
		static void LoadAllIBLCubemaps(const std::string& directory);

		static const std::vector<std::string>& GetAllCubeMaps() { return mCubemapNames; }
	private:
		inline static std::unordered_map<std::string, Ref<Texture2D>> mTexture2DMap;
		inline static std::vector<std::string> mCubemapNames;
		inline static std::unordered_map<std::string, Ref<TextureCubemap>> mTextureCubeMap;
		inline static std::unordered_map<std::string, Ref<Mesh>> mMeshMap;
	};
}