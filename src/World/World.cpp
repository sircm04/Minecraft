#include "../pch.h"
#include "World.h"
#include "../Entity/Mob/Player.h"
#include "../Game.h"

World::World()
	: m_NoiseRandom(std::mt19937(std::random_device{}())),
	m_Noise(siv::PerlinNoise(m_NoiseRandom))
{
}

void World::Update(const WorldPosition2D& playerPosition)
{
	static std::optional<WorldPosition2D> prevPlayerPosition = std::nullopt;

	if (playerPosition != prevPlayerPosition)
	{
		UpdateChunks(playerPosition);
		prevPlayerPosition = playerPosition;
	}
}

void World::UpdateChunks(const WorldPosition2D& playerPosition)
{
	for (auto it = m_Chunks.begin(); it != m_Chunks.end(); ++it)
		if (glm::distance2(static_cast<glm::vec2>((it->first << 4) + 8), playerPosition) >= ((World::RENDER_DISTANCE + 1) << 4) * ((World::RENDER_DISTANCE + 1) << 4))
			it->second.m_ChunkState = ChunkState::Removed;

	const ChunkLocation playerChunkLocation = PosUtils::ConvertWorldPosToChunkLoc(playerPosition);

	auto loop = [&](unsigned int radius, auto code)
	{
		std::vector<std::future<void>> futures;

		int xPositiveDistance = radius + playerChunkLocation.x,
			zPositiveDistance = radius + playerChunkLocation.y,
			xNegativeDistance = -radius + playerChunkLocation.x,
			zNegativeDistance = -radius + playerChunkLocation.y;

		for (int x = xNegativeDistance; x < xPositiveDistance; ++x)
		{
			for (int z = zNegativeDistance; z < zPositiveDistance; ++z)
			{
				const ChunkLocation chunkLocation = { x, z },
					realChunkLocation = (chunkLocation << 4) + 8;

				if (glm::distance2(static_cast<glm::vec2>(realChunkLocation), playerPosition) < (radius << 4) * (radius << 4))
				{
					futures.emplace_back(m_Pool.Enqueue([code, chunkLocation]
					{
						code(chunkLocation);
					}));
				}
			}
		}

		for (auto& future : futures)
			future.wait();
	};

	loop(World::RENDER_DISTANCE + 1, [&](const ChunkLocation& chunkLocation)
	{
		if (m_Chunks.find(chunkLocation) == m_Chunks.end())
		{
			Chunk chunk;
			chunk.Generate(m_Noise, chunkLocation);
			SetChunk(chunkLocation, std::move(chunk));
		}
	});

	loop(World::RENDER_DISTANCE, [&](const ChunkLocation& chunkLocation)
	{
		Chunk* chunk = GetChunk(chunkLocation);

		if (chunk && chunk->m_ChunkState == ChunkState::Generated)
			chunk->GenerateMesh(*this, chunkLocation);
	});
}

void World::RenderChunks(const ViewFrustum& frustum)
{
	auto it = m_Chunks.begin();
	while (it != m_Chunks.end())
	{
		switch (it->second.m_ChunkState)
		{
		case ChunkState::Removed:
			{
				std::lock_guard lock(m_Mutex);
				it = m_Chunks.erase(it);
			}
			continue;
		case ChunkState::GeneratedMesh:
			it->second.BufferMesh();
		case ChunkState::Buffered:
			it->second.Render(frustum, it->first);
		}

		++it;
	}
}

void World::SetBlock(const WorldPosition& position, const Block& block)
{
	Chunk* chunk = GetChunk(PosUtils::ConvertWorldPosToChunkLoc(position));

	if (chunk)
		chunk->SetBlock(PosUtils::ConvertWorldPosToChunkPos(position), block);
}

Block* World::GetBlock(const WorldPosition& position)
{
	Chunk* chunk = GetChunk(PosUtils::ConvertWorldPosToChunkLoc(position));

	return (chunk) ? chunk->GetBlock(PosUtils::ConvertWorldPosToChunkPos(position)) : nullptr;
}

const Block* World::GetBlock(const WorldPosition& position) const
{
	const Chunk* chunk = GetChunk(PosUtils::ConvertWorldPosToChunkLoc(position));

	return (chunk) ? chunk->GetBlock(PosUtils::ConvertWorldPosToChunkPos(position)) : nullptr;
}

Block* World::GetHighestBlock(const WorldPosition2D& position)
{
	Chunk* chunk = GetChunk(PosUtils::ConvertWorldPosToChunkLoc(position));

	ChunkPosition2D chunkPosition = PosUtils::ConvertWorldPosToChunkPos(position);
	return (chunk) ? chunk->GetHighestBlock(chunkPosition) : nullptr;
}

const Block* World::GetHighestBlock(const WorldPosition2D& position) const
{
	const Chunk* chunk = GetChunk(PosUtils::ConvertWorldPosToChunkLoc(position));

	ChunkPosition2D chunkPosition = PosUtils::ConvertWorldPosToChunkPos(position);
	return (chunk) ? chunk->GetHighestBlock(chunkPosition) : nullptr;
}

std::optional<uint8_t> World::GetHighestBlockYPos(const WorldPosition2D& position) const
{
	const Chunk* chunk = GetChunk(PosUtils::ConvertWorldPosToChunkLoc(position));

	ChunkPosition2D chunkPosition = PosUtils::ConvertWorldPosToChunkPos(position);
	return (chunk) ? chunk->GetHighestBlockYPos(chunkPosition) : std::nullopt;
}

void World::SetChunk(const ChunkLocation& position, Chunk&& chunk)
{
	std::lock_guard lock(m_Mutex);
	m_Chunks.emplace(position, std::move(chunk));
}

Chunk* World::GetChunk(const ChunkLocation& position)
{
	const auto& found = m_Chunks.find(position);

	return (found == m_Chunks.end()) ? nullptr : &found->second;
}

const Chunk* World::GetChunk(const ChunkLocation& position) const
{
	const auto& found = m_Chunks.find(position);

	return (found == m_Chunks.end()) ? nullptr : &found->second;
}

std::unordered_map<glm::ivec2, Chunk*> World::GetNeighboringChunks(const WorldPosition& position)
{
	std::unordered_map<glm::ivec2, Chunk*> chunks;

	const ChunkLocation chunkLocation = PosUtils::ConvertWorldPosToChunkLoc(position);
	const ChunkPosition chunkPosition = PosUtils::ConvertWorldPosToChunkPos(position);

	auto emplaceChunk = [&](bool boolean, const glm::ivec2& chunkPos)
	{
		if (boolean)
			chunks.emplace(chunkPos, GetChunk(chunkPos));
	};

	emplaceChunk(chunkPosition.x == 0, { chunkLocation.x - 1, chunkLocation.y });
	emplaceChunk(chunkPosition.z == 0, { chunkLocation.x, chunkLocation.y - 1 });
	emplaceChunk(chunkPosition.x == (Chunk::CHUNK_WIDTH - 1), { chunkLocation.x + 1, chunkLocation.y });
	emplaceChunk(chunkPosition.z == (Chunk::CHUNK_DEPTH - 1), { chunkLocation.x, chunkLocation.y + 1 });

	emplaceChunk(chunkPosition.x == 0 && chunkPosition.z == 0, { chunkLocation.x - 1, chunkLocation.y - 1 });
	emplaceChunk(chunkPosition.x == (Chunk::CHUNK_WIDTH - 1) && chunkPosition.z == (Chunk::CHUNK_DEPTH - 1), { chunkLocation.x + 1, chunkLocation.y + 1 });
	emplaceChunk(chunkPosition.x == 0 && chunkPosition.z == (Chunk::CHUNK_DEPTH - 1), { chunkLocation.x - 1, chunkLocation.y + 1 });
	emplaceChunk(chunkPosition.z == 0 && chunkPosition.x == (Chunk::CHUNK_WIDTH - 1), { chunkLocation.x + 1, chunkLocation.y - 1 });

	return chunks;
}

bool World::IsChunkLoaded(const ChunkLocation& location)
{
	std::lock_guard lock(m_Mutex);
	return m_Chunks.find(location) != m_Chunks.end();
}
