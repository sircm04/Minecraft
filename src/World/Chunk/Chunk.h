#pragma once

#include "../../Positions.h"
#include "../Block/Block.h"
#include "../../Renderer/Mesh.h"

class ViewFrustum;
class World;

enum class ChunkState : uint8_t
{
	Ungenerated,
	Generated,
	GeneratingMesh,
	GeneratedMesh,
	Buffered,
	Removed
};

class Chunk
{
public:
	static constexpr inline uint8_t CHUNK_WIDTH = 16, CHUNK_HEIGHT = 255, CHUNK_DEPTH = 16;

	ChunkState m_ChunkState;

private:
	struct Vertex
	{
		glm::vec3 position,
			texcoords,
			normal;

		unsigned int ao;
	};

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

	uint8_t CalculateVertexAO(uint8_t side1, uint8_t side2, uint8_t corner)
	{
		static VertexAOCache cache;
		return cache[(side1 << 2) | (side2 << 1) | corner];
	}

	std::vector<Block> m_Blocks;
	std::unique_ptr<Mesh<Vertex>> m_Mesh;

public:
	Chunk();

	void Generate(const siv::PerlinNoise& noise, const ChunkLocation& location);

	void GenerateMesh(World& world, const ChunkLocation& location);
	void AddFaceToMesh(World& world, const BlockFace& face, const WorldPosition& position, float texture, bool doAO = true);
	void BufferMesh();

	void Render(const ViewFrustum& frustum, const ChunkLocation& location);

	void SetBlock(const ChunkPosition& position, const Block& block);
	Block* GetBlock(const ChunkPosition& position);
	const Block* GetBlock(const ChunkPosition& position) const;
	Block* GetHighestBlock(const ChunkPosition2D& position);
	const Block* GetHighestBlock(const ChunkPosition2D& position) const;
	std::optional<uint8_t> GetHighestBlockYPos(const ChunkPosition2D& position) const;

	uint16_t PositionToIndex(const ChunkPosition& position) const;

	static constexpr bool IsInBounds(const glm::uvec3& position)
	{
		return (position.x >= 0 && position.x < Chunk::CHUNK_WIDTH
			&& position.y >= 0 && position.y < Chunk::CHUNK_HEIGHT
			&& position.z >= 0 && position.z < Chunk::CHUNK_DEPTH);
	}
};
