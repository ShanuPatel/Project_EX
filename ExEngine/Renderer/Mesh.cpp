#include "PCH.h"
#include "Mesh.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Core/AssetManager.h"
#include "MikkTSpaceHelper.h"
#include "Material.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

namespace EX
{
	Mesh::Mesh(const char* filepath)
	{
		Load(filepath);
	}

	bool Mesh::GetExtension()
	{
		std::string extension = std::filesystem::path(mFilepath).extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		EX_CORE_INFO("Extension{0}:", extension);
		if (extension == ".fbx") {
			bModelType = true;
			EX_CORE_INFO("Loading: fbx Model");
			return bModelType;
		}
		else if (extension == ".gltf" || extension == ".glb") 
		{
			bModelType = false;
			EX_CORE_INFO("Loading: gltf Model");
			return bModelType;
		}

		std::cout << "Unknown extension.\n";
		return false;
	}

	void Mesh::Load(const char* filepath)
	{
		EX_CORE_INFO("ASSIMP LOGS Start: Loading Mesh");
		Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
		Assimp::LogStream* stderrStream = Assimp::LogStream::createDefaultStream(aiDefaultLogStream_STDERR);
		Assimp::DefaultLogger::get()->attachStream(stderrStream, Assimp::Logger::NORMAL | Assimp::Logger::DEBUGGING | Assimp::Logger::VERBOSE);

		Assimp::Importer importer;
		importer.SetPropertyFloat("PP_GSN_MAX_SMOOTHING_ANGLE", 80.0f);
		uint32_t meshImportFlags = 0;

		std::string filePath = std::string(filepath);
		auto lastDot = filePath.find_last_of(".");
		std::string name = filePath.substr(lastDot + 1, filePath.size() - lastDot);
		if (name != "GLbin")
		{
			meshImportFlags |= aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_SortByPType | aiProcess_GenNormals
				| aiProcess_GenUVCoords | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | aiProcess_GlobalScale | aiProcess_ImproveCacheLocality | aiProcess_ValidateDataStructure;
		}

		const aiScene* scene = importer.ReadFile(filepath, meshImportFlags);
		//auto ai_scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), 0, "glb");
		if (!scene)
		{
			EX_CORE_ERROR("Could not import the file: {0}. Error: {1}", filepath, importer.GetErrorString());
			return;
		}

		mFilepath = filePath;
		ProcessNode(scene->mRootNode, scene, filepath);
		//mName = StringUtils::GetName(filepath);

		Assimp::DefaultLogger::kill();
		EX_CORE_INFO("ASSIMP LOGS Ends");
	}

	MeshVA& Mesh::GetMeshData(uint32_t index)
	{
		EX_CORE_ASSERT(index < mMeshes.size(), "MeshData index out of bounds");

		return mMeshes[index];
	}

	void Mesh::ProcessNode(aiNode* node, const aiScene* scene, const char* filepath)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			uint32_t meshIndex = node->mMeshes[i]; // Get actual mesh index from node
			aiMesh* mesh = scene->mMeshes[meshIndex];
			const char* nodeName = node->mName.C_Str();

			mMeshes.emplace_back(ProcessMesh(mesh, scene, filepath, nodeName, meshIndex)); //pass meshIndex
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, filepath);
		}
	}

	MeshVA Mesh::ProcessMesh(aiMesh* mesh, const aiScene* scene, const char* filepath, const char* nodeName, uint32_t meshIndex)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		vertices.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces * 3); // Most meshes have triangles, so we preallocate

		// Iterate over all vertices
		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex v;
			auto& vertexPosition = mesh->mVertices[i];
			v.Position = { vertexPosition.x, vertexPosition.y, vertexPosition.z };

			if (mesh->mTextureCoords[0])
				v.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			else
				v.TexCoord = { 0.0f, 0.0f };

			auto& normal = mesh->mNormals[i];
			v.Normal = { normal.x, normal.y, normal.z };

			if (mesh->mTangents && mesh->mBitangents)
			{
				auto& tangent = mesh->mTangents[i];
				auto& bitangent = mesh->mBitangents[i];

				v.Tangent = { tangent.x, tangent.y, tangent.z };
				v.Bitangent = { bitangent.x, bitangent.y, bitangent.z };
			}
			else
			{
				// Tangents/Bitangents missing - will compute later
				v.Tangent = { 0.0f, 0.0f, 0.0f };
				v.Bitangent = { 0.0f, 0.0f, 0.0f };
			}

			if (uniqueVertices.count(v) == 0)
			{
				uniqueVertices[v] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(v);
			}
		}

		// Process indices
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
			{
				uint32_t index = face.mIndices[j];
				indices.push_back(index);
			}
		}

		// Compute missing tangents and bitangents
		if (!mesh->mTangents || !mesh->mBitangents)
		{
			GenerateTangentsMikkTSpace(vertices, indices);
		}

		// Create GPU Buffers
		Ref<VertexArray> vertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(reinterpret_cast<float*>(vertices.data()), sizeof(Vertex) * vertices.size());

		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Bitangent" },
			});
		vertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());
		vertexArray->SetIndexBuffer(indexBuffer);

		{
			/*Materials*/
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			aiString name;
			material->Get(AI_MATKEY_NAME, name);

			std::string matName = name.C_Str();
			if (matName.empty())
				matName = std::string("Unnamed_") + std::to_string(meshIndex);

			Ref<Material> mat = CreateRef<Material>();

			MeshVA meshData(nodeName, mat, vertexArray);
			SetupMaterials(scene, mesh, filepath, meshData);
			return meshData;
		}
	}

	std::vector<Ref<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const char* filepath)
	{
		std::filesystem::path modelPath(filepath);
		std::filesystem::path dir = modelPath.parent_path();

		std::vector<Ref<Texture2D>> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			// Assimp texture filename (relative to model directory)
			std::filesystem::path texPath = dir / str.C_Str();
			texPath = texPath.lexically_normal();

			Ref<Texture2D> texture = AssetManager::GetTexture2D(texPath.string());
			textures.push_back(texture);
		}
		return textures;
	}

	std::vector<Ref<Texture2D>> LoadTexturesFromMemory(const aiTexture* texture)
	{
		std::vector<Ref<Texture2D>> textures;
		texture->pcData;
		return textures;
	}

	void Mesh::SetupMaterials(const aiScene* scene, aiMesh* mesh, const char* filepath, const MeshVA& meshData)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// Extract PBR scalar values
		float metallic = 1.0f, roughness = 1.0f;
		material->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
		material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);

		// Load all relevant textures
		auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, filepath);
		auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, filepath);
		auto normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, filepath);
		auto heightMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, filepath);
		auto metallicRoughnessMaps = LoadMaterialTextures(material, aiTextureType_METALNESS, filepath);
		auto roughnessMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, filepath);
		auto aoMaps = LoadMaterialTextures(material, aiTextureType_LIGHTMAP, filepath);
		auto emissiveMaps = LoadMaterialTextures(material, aiTextureType_EMISSIVE, filepath);
		auto ClearCoatMap = LoadMaterialTextures(material, aiTextureType_CLEARCOAT, filepath);
		
		if(!GetExtension())
		{
			if (!diffuseMaps.empty())
				meshData.Mat->SetAlbedoTexture(diffuseMaps[0]);
			if (!normalMaps.empty())
				meshData.Mat->SetNormalTexture(normalMaps[0]);
			if (!metallicRoughnessMaps.empty())
				meshData.Mat->SetMetallicRoughnessTexture(metallicRoughnessMaps[0]);
			if (!roughnessMaps.empty())
				meshData.Mat->SetRoughnessTexture(roughnessMaps[0]);
			if (!aoMaps.empty())
				meshData.Mat->SetAOTexture(aoMaps[0]);
			if (!emissiveMaps.empty())
				meshData.Mat->SetEmissiveTexture(emissiveMaps[0]);
		}
		else
		{
			if (!diffuseMaps.empty())
				meshData.Mat->SetAlbedoTexture(diffuseMaps[0]);
			if (normalMaps.empty())
				meshData.Mat->SetNormalTexture(AssetManager::GetTexture2D("bin/assets/models/fbx/Cerberus/Textures/Cerberus_N.tga"));
			if (metallicRoughnessMaps.empty())
				meshData.Mat->SetMetallicRoughnessTexture(AssetManager::GetTexture2D("bin/assets/models/fbx/Cerberus/Textures/Cerberus_M.tga"));
			if (roughnessMaps.empty())
				meshData.Mat->SetRoughnessTexture(AssetManager::GetTexture2D("bin/assets/models/fbx/Cerberus/Textures/Cerberus_R.tga"));

		}

	}

}