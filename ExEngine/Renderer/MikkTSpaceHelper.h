#pragma once

#include "mikktspace.h"

namespace EX
{

	struct Vertex
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec2 TexCoord = glm::vec2(0.0f);
		glm::vec3 Normal = glm::vec3(0.0f);
		glm::vec3 Tangent = glm::vec3(0.0f);
		glm::vec3 Bitangent = glm::vec3(0.0f);

		bool operator==(const Vertex& other) const
		{
			return Position == other.Position &&
				TexCoord == other.TexCoord &&
				Normal == other.Normal &&
				Tangent == other.Tangent &&
				Bitangent == other.Bitangent;
		}
	};
}

namespace std
{
	template<>
	struct hash<glm::vec2>
	{
		size_t operator()(const glm::vec2& v) const
		{
			size_t h1 = std::hash<float>{}(v.x);
			size_t h2 = std::hash<float>{}(v.y);
			return h1 ^ (h2 << 1);
		}
	};

	template<>
	struct hash<glm::vec3>
	{
		size_t operator()(const glm::vec3& v) const
		{
			size_t h1 = std::hash<float>{}(v.x);
			size_t h2 = std::hash<float>{}(v.y);
			size_t h3 = std::hash<float>{}(v.z);
			return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
		}
	};
}

namespace std
{
	template<> struct hash<EX::Vertex>
	{
		size_t operator()(const EX::Vertex& vertex) const noexcept
		{
			const std::size_t h1 = std::hash<glm::vec3>{}(vertex.Position);
			const std::size_t h2 = std::hash<glm::vec2>{}(vertex.TexCoord);
			const std::size_t h3 = std::hash<glm::vec3>{}(vertex.Normal);
			const std::size_t h4 = std::hash<glm::vec3>{}(vertex.Tangent);
			const std::size_t h5 = std::hash<glm::vec3>{}(vertex.Bitangent);
			return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
		}
	};
}

namespace EX
{
	struct MikkTSpaceData
	{
		std::vector<Vertex>* Vertices = nullptr;
		std::vector<uint32_t>* Indices = nullptr;
	};

	// MikkTSpace Callbacks
	inline int GetNumFaces(const SMikkTSpaceContext* context)
	{
		auto* data = static_cast<MikkTSpaceData*>(context->m_pUserData);
		return static_cast<int>(data->Indices->size()) / 3;
	}

	inline int GetNumVerticesOfFace(const SMikkTSpaceContext*, int)
	{
		return 3;
	}

	inline void GetPosition(const SMikkTSpaceContext* context, float outPos[3], int face, int vert)
	{
		auto* data = static_cast<MikkTSpaceData*>(context->m_pUserData);
		const auto& vertex = (*data->Vertices)[(*data->Indices)[face * 3 + vert]];
		outPos[0] = vertex.Position.x;
		outPos[1] = vertex.Position.y;
		outPos[2] = vertex.Position.z;
	}

	inline void GetNormal(const SMikkTSpaceContext* context, float outNorm[3], int face, int vert)
	{
		auto* data = static_cast<MikkTSpaceData*>(context->m_pUserData);
		const auto& vertex = (*data->Vertices)[(*data->Indices)[face * 3 + vert]];
		outNorm[0] = vertex.Normal.x;
		outNorm[1] = vertex.Normal.y;
		outNorm[2] = vertex.Normal.z;
	}

	inline void GetTexCoord(const SMikkTSpaceContext* context, float outUV[2], int face, int vert)
	{
		auto* data = static_cast<MikkTSpaceData*>(context->m_pUserData);
		const auto& vertex = (*data->Vertices)[(*data->Indices)[face * 3 + vert]];
		outUV[0] = vertex.TexCoord.x;
		outUV[1] = vertex.TexCoord.y;
	}

	inline void SetTSpaceBasic(const SMikkTSpaceContext* context, const float tangent[3], float sign, int face, int vert)
	{
		auto* data = static_cast<MikkTSpaceData*>(context->m_pUserData);
		auto& vertex = (*data->Vertices)[(*data->Indices)[face * 3 + vert]];

		vertex.Tangent = glm::vec3(tangent[0], tangent[1], tangent[2]);
		vertex.Bitangent = glm::normalize(glm::cross(vertex.Normal, vertex.Tangent)) * sign;
	}

	inline void GenerateTangentsMikkTSpace(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
	{
		MikkTSpaceData data{ &vertices, &indices };

		SMikkTSpaceInterface iface{};
		iface.m_getNumFaces = GetNumFaces;
		iface.m_getNumVerticesOfFace = GetNumVerticesOfFace;
		iface.m_getPosition = GetPosition;
		iface.m_getNormal = GetNormal;
		iface.m_getTexCoord = GetTexCoord;
		iface.m_setTSpaceBasic = SetTSpaceBasic;

		SMikkTSpaceContext context{};
		context.m_pInterface = &iface;
		context.m_pUserData = &data;

		genTangSpaceDefault(&context);
	}
}