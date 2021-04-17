#include "../../pch.h"
// FIXME: Including this causes preprocessor to put the
// Chunk.h file below World.h despite it including Chunk.h
//#include "Chunk.h"

#include "../World.h"

Chunk::Chunk() noexcept
{
}

void Chunk::Generate(siv::PerlinNoise* noise, const glm::ivec2& chunkPosition) noexcept
{
	m_ChunkState = ChunkState::Ungenerated;

	m_Blocks.resize(Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT * Chunk::CHUNK_DEPTH);

	int realChunkX = (chunkPosition.x << 4),
		realChunkZ = (chunkPosition.y << 4);

	for (uint8_t x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		for (uint8_t z = 0; z < Chunk::CHUNK_DEPTH; ++z)
		{
			const double random = noise->noise0_1(((realChunkX + x) * 0.025), ((realChunkZ + z) * 0.025)) * 50;
			const uint8_t grassHeight = (static_cast<uint8_t>(Chunk::CHUNK_HEIGHT * 0.33f) >> 2) + random,
				dirtHeight = (grassHeight - 3);

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

void Chunk::GenerateTrees(World* world, const glm::ivec2& chunkPosition) noexcept
{
	for (uint8_t x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		for (uint8_t z = 0; z < Chunk::CHUNK_DEPTH; ++z)
		{
			const uint8_t y = GetHighestBlockYPosition({ x, z });

			if (rand() % 10 + 1 == 1 && GetBlock({ x, y, z })->m_BlockType != BlockType::Leaves)
			{
				GenerateTree(world,
				{
					(chunkPosition.x << 4) + x,
					y,
					(chunkPosition.y << 4) + z
				});
			}
		}
	}

	m_ChunkState = ChunkState::Complete;
}

void Chunk::GenerateTree(World* world, const glm::ivec3& worldPosition) noexcept
{
	for (int8_t x = -1; x < 2; ++x)
		for (int8_t z = -2; z < 3; ++z)
			world->SetBlock({ worldPosition.x + x, worldPosition.y + 4, worldPosition.z + z }, { BlockType::Leaves });

	for (int8_t x = -2; x < 3; ++x)
		for (int8_t z = -1; z < 2; ++z)
			world->SetBlock({ worldPosition.x + x, worldPosition.y + 4, worldPosition.z + z }, { BlockType::Leaves });

	for (int8_t x = -1; x < 2; ++x)
		for (int8_t z = -1; z < 2; ++z)
			for (int8_t y = 5; y < 6; ++y)
				world->SetBlock({ worldPosition.x + x, worldPosition.y + y, worldPosition.z + z }, { BlockType::Leaves });

	for (int8_t x = -1; x < 2; ++x)
			world->SetBlock({ worldPosition.x + x, worldPosition.y + 6, worldPosition.z }, { BlockType::Leaves });

	for (int8_t z = -1; z < 2; ++z)
		world->SetBlock({ worldPosition.x , worldPosition.y + 6, worldPosition.z + z }, { BlockType::Leaves });

	for (int8_t i = 1; i < 5; ++i)
		world->SetBlock({ worldPosition.x, worldPosition.y + i, worldPosition.z }, { BlockType::Log });

	world->SetBlock({ worldPosition.x + 2, worldPosition.y + 5, worldPosition.z }, { BlockType::Leaves });
	world->SetBlock({ worldPosition.x - 2, worldPosition.y + 5, worldPosition.z }, { BlockType::Leaves });
	world->SetBlock({ worldPosition.x, worldPosition.y + 5, worldPosition.z + 2 }, { BlockType::Leaves });
	world->SetBlock({ worldPosition.x, worldPosition.y + 5, worldPosition.z - 2 }, { BlockType::Leaves });
}

void Chunk::GenerateMesh(const World* world, const glm::ivec2& chunkPosition) noexcept
{
	m_ChunkMesh.Generate(this, world, chunkPosition);
}

void Chunk::SetBlock(const glm::uvec3& position, const Block& block) noexcept
{
	m_Blocks[PositionToIndex(position)] = block;
}

Block* Chunk::GetBlock(const glm::uvec3& position) noexcept
{
	return &m_Blocks[PositionToIndex(position)];
}

const Block* Chunk::GetBlock(const glm::uvec3& position) const noexcept
{
	return &m_Blocks[PositionToIndex(position)];
}

Block* Chunk::GetBlockInBounds(const glm::uvec3& position) noexcept
{
	return ((IsInBounds(position)) ? &m_Blocks[PositionToIndex(position)] : nullptr);
}

const Block* Chunk::GetBlockInBounds(const glm::uvec3& position) const noexcept
{
	return ((IsInBounds(position)) ? &m_Blocks[PositionToIndex(position)] : nullptr);
}

uint8_t Chunk::GetHighestBlockYPosition(const glm::ivec2& position) const noexcept
{
	for (uint8_t y = Chunk::CHUNK_HEIGHT - 1; y > 0; --y)
		if (GetBlock({ position.x, y, position.y })->GetBlockTypeData().isSolid)
			return y;

	return -1;
}
