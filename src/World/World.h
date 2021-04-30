#pragma once

#include "../Positions.h"
#include "Chunk/Chunk.h"
#include "../Utils/ThreadPool.h"

class World
{
private:
	std::unordered_map<glm::ivec2, Chunk> m_Chunks;

	ThreadPool m_Pool;
	std::mutex m_Mutex;

public:
	static inline unsigned int RENDER_DISTANCE = 24;

	void Update(const ChunkLocation& playerChunkLocation);
	void UpdateChunks(const ChunkLocation& playerChunkLocation);

	void SetBlock(const WorldPosition& position, const Block& block);
	Block* GetBlock(const WorldPosition& position);
	const Block* GetBlock(const WorldPosition& position) const;
	Block* GetHighestBlock(int x, int z);
	const Block* GetHighestBlock(int x, int z) const;
	std::optional<uint8_t> GetHighestBlockYPos(int x, int z) const;

	void SetChunk(const ChunkLocation&, Chunk&& chunk);
	Chunk* GetChunk(const ChunkLocation& position);
	const Chunk* GetChunk(const ChunkLocation& position) const;
};
