#include "../../pch.h"
#include "Chunk.h"

void Chunk::Generate()
{
	m_ChunkState = ChunkState::Ungenerated;

	m_Model = std::make_unique<Model<>>();

	m_Blocks.resize(Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT * Chunk::CHUNK_DEPTH);

	for (uint8_t x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		for (uint8_t z = 0; z < Chunk::CHUNK_DEPTH; ++z)
		{
			for (uint8_t y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
			{
				if (y <= 50)
					SetBlock({ x, y, z }, { BlockType::Grass });
				else if (y > 50)
					SetBlock({ x, y, z }, { BlockType::Air });
			}
		}
	}

	m_ChunkState = ChunkState::Generated;
}

void Chunk::GenerateMesh(const ChunkLocation& chunkLocation)
{
	for (uint8_t x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		for (uint8_t z = 0; z < Chunk::CHUNK_DEPTH; ++z)
		{
			for (uint8_t y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
			{
				WorldPosition position = { x + chunkLocation.x * Chunk::CHUNK_WIDTH, y, z + chunkLocation.y * Chunk::CHUNK_DEPTH };

				if (y == 50)
					AddFaceToMesh(TOP_BLOCK_FACE_VERTICES, position, 0);
			}
		}
	}
}

void Chunk::AddFaceToMesh(const BlockFace& blockFace, const WorldPosition& position, float face)
{
	m_Model->AddMesh({
		{
			blockFace[0] + position.x,
			blockFace[1] + position.y,
			blockFace[2] + position.z,
			blockFace[3],
			blockFace[4],
			blockFace[5] + face,
			blockFace[6],
			blockFace[7],
			blockFace[8]
		},
		{
			0, 1, 2, 2, 3, 0
		}
	});
}

void Chunk::BufferMesh(VertexArray& vertexArray)
{
	m_Model->Buffer(vertexArray);
}

void Chunk::Render()
{
	m_Model->Render();
}

void Chunk::SetBlock(const ChunkPosition& position, const Block& block)
{
	m_Blocks.at(PositionToIndex(position)) = block;
}

Block* Chunk::GetBlock(const ChunkPosition& position)
{
	return &m_Blocks.at(PositionToIndex(position));
}

const Block* Chunk::GetBlock(const ChunkPosition& position) const
{
	return &m_Blocks.at(PositionToIndex(position));
}

Block* Chunk::GetHighestBlock(uint8_t x, uint8_t z)
{
	for (uint8_t y = Chunk::CHUNK_HEIGHT - 1; y > 0; --y)
	{
		Block* block = GetBlock({ x, y, z });
		if (block->type != BlockType::Air)
			return block;
	}

	return nullptr;
}

const Block* Chunk::GetHighestBlock(uint8_t x, uint8_t z) const
{
	for (uint8_t y = Chunk::CHUNK_HEIGHT - 1; y > 0; --y)
	{
		const Block* block = GetBlock({ x, y, z });
		if (block->type != BlockType::Air)
			return block;
	}

	return nullptr;
}

std::optional<uint8_t> Chunk::GetHighestBlockYPos(uint8_t x, uint8_t z) const
{
	for (uint8_t y = Chunk::CHUNK_HEIGHT - 1; y > 0; --y)
		if (GetBlock({ x, y, z })->type != BlockType::Air)
			return y;

	return std::nullopt;
}

uint16_t Chunk::PositionToIndex(const ChunkPosition& position) const
{
	return (static_cast<uint16_t>(position.y) << 8) | (static_cast<uint16_t>(position.z) << 4) | static_cast<uint16_t>(position.x);
}
