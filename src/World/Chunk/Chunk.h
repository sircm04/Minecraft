#pragma once

#include "../../Positions.h"
#include "../Block/Block.h"
#include "../../Renderer/Model.h"

enum class ChunkState : uint8_t
{
	Ungenerated,
	Generated,
	Buffered,
	Removed
};

class Chunk
{
public:
	static constexpr uint8_t CHUNK_WIDTH = 16, CHUNK_HEIGHT = 255, CHUNK_DEPTH = 16;

	ChunkState m_ChunkState;

private:
	struct Vertex
	{
		glm::vec3 position,
			texcoords,
			normal;

		uint8_t ao;
	};

	std::vector<Block> m_Blocks;
	std::unique_ptr<Model<Vertex>> m_Model;

public:
	void Generate();

	void GenerateMesh(const ChunkLocation& chunkLocation);
	void AddFaceToMesh(const BlockFace& face, const WorldPosition& position, float texture);
	void BufferMesh(VertexArray& vertexArray);

	void Render();

	void SetBlock(const ChunkPosition& position, const Block& block);
	Block* GetBlock(const ChunkPosition& position);
	const Block* GetBlock(const ChunkPosition& position) const;
	Block* GetHighestBlock(uint8_t x, uint8_t z);
	const Block* GetHighestBlock(uint8_t x, uint8_t z) const;
	std::optional<uint8_t> GetHighestBlockYPos(uint8_t x, uint8_t z) const;

	uint16_t PositionToIndex(const ChunkPosition& position) const;
};
