#include "pch.h"
#include "Chunk.h"

#include "World.h"

void Chunk::Generate(siv::PerlinNoise* noise, const glm::ivec2& chunkPosition)
{
	m_ChunkState = ChunkState::Ungenerated;

	m_Blocks.resize(Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT * Chunk::CHUNK_DEPTH);

	int realChunkX = (chunkPosition.x * Chunk::CHUNK_WIDTH),
		realChunkZ = (chunkPosition.y * Chunk::CHUNK_DEPTH);

	for (uint8_t x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		for (uint8_t z = 0; z < Chunk::CHUNK_DEPTH; ++z)
		{
			double random = (noise->noise0_1((double) (x + realChunkX) / (Chunk::CHUNK_WIDTH / 0.4f), (double) (z + realChunkZ) / (Chunk::CHUNK_DEPTH / 0.4f)) * 50);
			uint8_t grassHeight = (Chunk::GRASS_HEIGHT / 2) + random, dirtHeight = (grassHeight - 3);

			for (uint8_t y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
			{
				if (y > grassHeight)
					SetBlock({ x, y, z }, { BlockType::Air });
				else if (y == grassHeight)
					SetBlock({ x, y, z }, { BlockType::Grass });
				else if (y >= dirtHeight)
					SetBlock({ x, y, z }, { BlockType::Dirt });
				else if (y > 0)
					SetBlock({ x, y, z }, { BlockType::Stone });
				else
					SetBlock({ x, y, z }, { BlockType::Bedrock });
			}
		}
	}

	m_ChunkState = ChunkState::Generated;
}

void Chunk::GenerateMesh(World* world, const glm::ivec2& chunkPosition)
{
	m_ChunkMesh.Generate(this, world, chunkPosition);
}

bool Chunk::IsInBounds(const glm::ivec3& position)
{
	return (position.x >= 0 && position.x < Chunk::CHUNK_WIDTH
		&& position.y >= 0 && position.y < Chunk::CHUNK_HEIGHT
		&& position.z >= 0 && position.z < Chunk::CHUNK_DEPTH);
}

bool Chunk::SetBlock(const glm::ivec3& position, const Block& block)
{
	if (!IsInBounds(position))
		return false;

	m_Blocks.at(position.x + position.y * Chunk::CHUNK_WIDTH + position.z * Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT) = block;
	return true;
}

Block* Chunk::GetBlock(const glm::ivec3& position)
{
	if (IsInBounds(position))
		return &m_Blocks.at(position.x + position.y * Chunk::CHUNK_WIDTH + position.z * Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT);

	return nullptr;
}

const Block* Chunk::GetBlock(const glm::ivec3& position) const
{
	if (IsInBounds(position))
		return &m_Blocks.at(position.x + position.y * Chunk::CHUNK_WIDTH + position.z * Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT);

	return nullptr;
}

int Chunk::GetHighestBlockYPosition(int x, int z) const
{
	for (int y = Chunk::CHUNK_HEIGHT; y > 0; y--)
	{
		if (GetBlock({ x, y, z })->GetBlockTypeData().isSolid)
			return y;
	}

	return -1;
}