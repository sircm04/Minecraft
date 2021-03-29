#pragma once

#include "ChunkMesh.h"
#include "../../Physics/AABB.h"

class World;

enum class ChunkState : uint8_t
{
	Ungenerated,
	Generated,
	Complete,
	Removed
};

class Chunk
{
public:
	static constexpr uint8_t CHUNK_WIDTH = 16, CHUNK_HEIGHT = 255, CHUNK_DEPTH = 16;

private:
	std::vector<Block> m_Blocks;

	ChunkState m_ChunkState = ChunkState::Ungenerated;

	static constexpr uint16_t PositionToIndex(const glm::uvec3& position) noexcept
	{
		return (static_cast<uint16_t>(position.y) << 8) | (static_cast<uint16_t>(position.z) << 4) | static_cast<uint16_t>(position.x);
	}

public:
	ChunkMesh m_ChunkMesh;

	Chunk() noexcept;
	~Chunk() = default;
	Chunk(Chunk&&) noexcept = default;
	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	void Generate(siv::PerlinNoise* noise, const glm::ivec2& chunkPosition) noexcept;
	void GenerateTrees(World* world, const glm::ivec2& chunkPosition) noexcept;
	void GenerateTree(World* world, const glm::ivec3& worldPosition) noexcept;

	void GenerateMesh(const World* world, const glm::ivec2& chunkPosition) noexcept;

	static constexpr bool IsInBounds(const glm::uvec3& position) noexcept
	{
		return (position.x >= 0 && position.x < Chunk::CHUNK_WIDTH
			&& position.y >= 0 && position.y < Chunk::CHUNK_HEIGHT
			&& position.z >= 0 && position.z < Chunk::CHUNK_DEPTH);
	}

	void SetBlock(const glm::uvec3& position, const Block& block) noexcept;
	Block* GetBlock(const glm::uvec3& position) noexcept;
	const Block* GetBlock(const glm::uvec3& position) const noexcept;
	Block* GetBlockInBounds(const glm::uvec3& position) noexcept;
	const Block* GetBlockInBounds(const glm::uvec3& position) const noexcept;

	uint8_t GetHighestBlockYPosition(const glm::ivec2& position) const noexcept;

	const ChunkState* GetChunkState() const noexcept { return &m_ChunkState; }
	void SetRemoved() noexcept { m_ChunkState = ChunkState::Removed; }
};
