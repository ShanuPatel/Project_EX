#pragma once

struct aiScene;
struct aiNode;
struct aiMesh;

namespace EX
{

	class VertexArray;
	class Material;

	//VertexData
	struct MeshVA
	{
		std::string Name;
		Ref<VertexArray> Geometry;//VAO
		Ref<Material> Mat;
		MeshVA(const std::string& name, const Ref<Material>&mat, const Ref<VertexArray>& geometry)
			: Name(name), Mat(mat), Geometry(geometry)
		{
		}
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const char* filepath);
		virtual ~Mesh() = default;

		bool GetExtension();
		void Load(const char* filepath);

		MeshVA& GetMeshData(uint32_t index);
		const uint32_t GetMeshCount() const { return mMeshes.size(); }
		const char* GetName() const { return mName.c_str(); }

		const char* GetFilepath() const { return mFilepath.c_str(); }

		bool GetModelType()const { return bModelType; }
	private:
		void ProcessNode(aiNode* node, const aiScene* scene, const char* filepath);
		MeshVA ProcessMesh(aiMesh* mesh, const aiScene* scene, const char* filepath, const char* nodeName, uint32_t meshIndex);

		void ClearCoatTexture(aiMesh* mesh, const char* filepath);
		void SetupMaterials(const aiScene* scene, aiMesh* mesh, const char* filepath, const MeshVA& meshData);

	private:
		std::string mName;
		std::string mFilepath;
		std::vector<MeshVA> mMeshes;
		bool bModelType;
	};
}