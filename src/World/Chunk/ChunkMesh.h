#pragma once

#include "../../Renderer/Mesh.h"
#include "../Block/Block.h"
#include "../../Math/Frustum.h"

class World;
class Chunk;

inline constexpr std::array<float, 60> FRONT_BLOCK_FACE_VERTICES = {
	// front
	0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
};

inline constexpr std::array<float, 60> BACK_BLOCK_FACE_VERTICES {
	// back
	1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
};

inline constexpr std::array<float, 60> LEFT_BLOCK_FACE_VERTICES {
	// left
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

inline constexpr std::array<float, 60> RIGHT_BLOCK_FACE_VERTICES {
	// right
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

inline constexpr std::array<float, 60> TOP_BLOCK_FACE_VERTICES {
	// top
	1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
};

inline constexpr std::array<float, 60> BOTTOM_BLOCK_FACE_VERTICES {
	// bottom
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
};

enum class ChunkMeshState : uint8_t
{
	Ungenerated,
	Generated,
	Complete
};

class ChunkMesh
{
private:
	std::unique_ptr<Mesh> m_Mesh;

	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
	unsigned int m_IndicesIndex = 0;

	struct VertexAOCache
	{
		VertexAOCache() : m_Array{}
		{
			for (uint8_t i = 0; i < 8; i++)
			{
				uint8_t side1 = (i >> 2) & 1;
				uint8_t side2 = (i >> 1) & 1;
				uint8_t corner = (i >> 0) & 1;
				if (side1 && side2)
					m_Array[i] = 0;
				else
					m_Array[i] = 3 - (side1 + side2 + corner);
			}
		}

		constexpr uint8_t operator[](size_t i)
		{
			return m_Array[i];
		}

	private:
		uint8_t m_Array[8];
	};

	uint8_t CalculateVertexAO(uint8_t side1, uint8_t side2, uint8_t corner) noexcept
	{
		static VertexAOCache cache;
		return cache[(side1 << 2) | (side2 << 1) | corner];
	}

	bool DoesBlockExist(const World* world, glm::vec3 position) noexcept;

public:
	static const VertexBufferLayout* GetLayoutInstance() {
		static VertexBufferLayout* layoutInstance;
		if (!layoutInstance)
		{
			layoutInstance = new VertexBufferLayout();
			layoutInstance->Push<float>(3);
			layoutInstance->Push<float>(3);
			layoutInstance->Push<float>(3);
			layoutInstance->Push<float>(1);
		}
		return layoutInstance;
	}

	ChunkMeshState m_ChunkMeshState = ChunkMeshState::Ungenerated;

	ChunkMesh() noexcept = default;
	
	void Bind() const noexcept;
	void Unbind() const noexcept;

	void Generate(const Chunk* chunk, const World* world, const glm::ivec2& chunkPosition) noexcept;
	void AddBlockFace(const World* world, const glm::vec3& position, const std::array<float, 60>& vertices, float face, bool doAO) noexcept;
	void BufferMesh() noexcept;
	void ClearMesh() noexcept;

	void Render(const ViewFrustum& frustum, const glm::ivec2& chunkPosition) noexcept;
};
