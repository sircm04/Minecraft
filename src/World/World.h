#pragma once

#include "../Positions.h"
#include "Chunk/Chunk.h"
#include "../Utils/ThreadPool.h"

class World
{
private:
	std::unordered_map<ChunkLocation, Chunk> m_Chunks;

	ThreadPool m_Pool;
	mutable std::mutex m_Mutex;
	std::condition_variable_any m_Condition;

public:
	static inline unsigned int RENDER_DISTANCE = 24;

	siv::PerlinNoise m_Noise;
	std::mt19937 m_NoiseRandom;

	World();

	void Update(const WorldPosition2D& playerPosition);
	void UpdateChunks(const WorldPosition2D& playerPosition);

	void RenderChunks();

	void SetBlock(const WorldPosition& position, const Block& block);
	Block* GetBlock(const WorldPosition& position);
	const Block* GetBlock(const WorldPosition& position) const;
	Block* GetHighestBlock(int x, int z);
	const Block* GetHighestBlock(int x, int z) const;
	std::optional<uint8_t> GetHighestBlockYPos(int x, int z) const;

	void SetChunk(const ChunkLocation& location, Chunk&& chunk);
	Chunk* GetChunk(const ChunkLocation& location);
	const Chunk* GetChunk(const ChunkLocation& location) const;
};
