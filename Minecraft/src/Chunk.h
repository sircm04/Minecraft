#pragma once

#include "ChunkMesh.h"

class World;

enum class ChunkState : int8_t
{
	Removed,
	Ungenerated,
	Generated
};

class Chunk
{
private:
	std::vector<Block> m_Blocks;

	unsigned int m_BufferSubIndex = 0;

	ChunkState m_ChunkState = ChunkState::Ungenerated;

public:
	static const unsigned int CHUNK_WIDTH = 16, CHUNK_HEIGHT = 256, CHUNK_DEPTH = 16,

		CHUNK_WIDTH_M1 = (Chunk::CHUNK_WIDTH - 1), CHUNK_DEPTH_M1 = (Chunk::CHUNK_DEPTH - 1),

		CHUNK_X_MASK = 15, CHUNK_Z_MASK = 15,
		CHUNK_X_SHIFT = 4, CHUNK_Z_SHIFT = 4,

		GRASS_HEIGHT = (Chunk::CHUNK_HEIGHT - 171);

	ChunkMesh m_ChunkMesh;

	Chunk() = default;
	~Chunk() = default;
	Chunk(Chunk&&) = default;
	Chunk(const Chunk&) = delete;
	Chunk& operator=(Chunk&) = delete;

	void Generate(siv::PerlinNoise* noise, const glm::ivec2& chunkPosition);

	void GenerateMesh(World* world, const glm::ivec2& chunkPosition);

	static bool IsInBounds(const glm::ivec3& position);

	bool SetBlock(const glm::ivec3& position, const Block& block);
	Block* GetBlock(const glm::ivec3& position);

	int GetHighestBlockYPosition(int x, int z);

	inline const ChunkState* GetChunkState() const { return &m_ChunkState; }

	void SetRemoved() { m_ChunkState = ChunkState::Removed; }
};