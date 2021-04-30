#pragma once

#include "../../Positions.h"
#include "../Block/Block.h"
#include "../../Renderer/Mesh.h"

class Chunk
{
public:
	static constexpr uint8_t CHUNK_WIDTH = 16, CHUNK_HEIGHT = 255, CHUNK_DEPTH = 16;

private:
	std::vector<Block> m_Blocks;
	std::unique_ptr<Mesh> m_Mesh;

public:
	void Generate();
	void GenerateMesh(const ChunkLocation& chunkLocation);

	void SetBlock(const ChunkPosition& position, const Block& block);
	Block* GetBlock(const ChunkPosition& position);
	const Block* GetBlock(const ChunkPosition& position) const;
	Block* GetHighestBlock(uint8_t x, uint8_t z);
	const Block* GetHighestBlock(uint8_t x, uint8_t z) const;
	std::optional<uint8_t> GetHighestBlockYPos(uint8_t x, uint8_t z) const;

	uint16_t PositionToIndex(const ChunkPosition& position) const;
};
