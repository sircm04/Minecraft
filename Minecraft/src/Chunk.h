#pragma once

#include "ChunkMesh.h"
#include "AABB.h"

class World;

enum class ChunkState : uint8_t
{
	Removed,
	Ungenerated,
	Generated
};

class Chunk
{
public:
	static constexpr uint8_t CHUNK_WIDTH = 16, CHUNK_HEIGHT = 255, CHUNK_DEPTH = 16,
		
		CHUNK_WIDTH_M1 = (Chunk::CHUNK_WIDTH - 1), CHUNK_DEPTH_M1 = (Chunk::CHUNK_DEPTH - 1),

		CHUNK_X_MASK = 15, CHUNK_Z_MASK = 15,
		CHUNK_X_SHIFT = 4, CHUNK_Z_SHIFT = 4,

		GRASS_HEIGHT = (Chunk::CHUNK_HEIGHT - 171);

	static constexpr uint16_t CHUNK_BLOCKS = Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT * Chunk::CHUNK_DEPTH;

private:
	std::vector<Block> m_Blocks;

	ChunkState m_ChunkState = ChunkState::Ungenerated;

	std::shared_ptr<std::mutex> m_MutexLock;

public:
	ChunkMesh m_ChunkMesh;

	Chunk() noexcept;
	~Chunk() = default;
	Chunk(Chunk&&) noexcept = default;
	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	void Generate(siv::PerlinNoise* noise, const glm::ivec2& chunkPosition) noexcept;

	void GenerateMesh(const World* world, const glm::ivec2& chunkPosition) noexcept;

	static constexpr inline bool IsInBounds(const glm::ivec3& position) noexcept;

	bool SetBlock(const glm::ivec3& position, const Block& block) noexcept;
	Block* GetBlock(const glm::ivec3& position) noexcept;
	const Block* GetBlock(const glm::ivec3& position) const noexcept;

	int GetHighestBlockYPosition(int x, int z) const noexcept;

	inline const ChunkState* GetChunkState() const noexcept { return &m_ChunkState; }

	void SetRemoved() noexcept { m_ChunkState = ChunkState::Removed; }
};