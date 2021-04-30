#include "../pch.h"
#include "World.h"

void World::Update(const ChunkLocation& playerChunkLocation)
{
	static std::optional<ChunkLocation> prevPlayerChunkLocation = std::nullopt;

	if (playerChunkLocation != prevPlayerChunkLocation)
	{
		UpdateChunks(playerChunkLocation);

		const Block* block = GetBlock({ 8, 0, 8 });
		std::cout << (unsigned) block->type << std::endl;

		prevPlayerChunkLocation = playerChunkLocation;
	}
}

void World::UpdateChunks(const ChunkLocation& playerChunkLocation)
{
	auto loop = [&](unsigned int radius, auto code)
	{
		std::vector<std::future<void>> futures;

		for (int x = static_cast<signed>(playerChunkLocation.x - radius); x < static_cast<signed>(playerChunkLocation.x + radius); ++x)
		{
			for (int z = static_cast<signed>(playerChunkLocation.y - radius); z < static_cast<signed>(playerChunkLocation.y + radius); ++z)
			{
				const ChunkLocation chunkLocation = { x, z };

				if (ceil(glm::distance(static_cast<glm::vec2>(chunkLocation), static_cast<glm::vec2>(playerChunkLocation))) < radius)
				{
					futures.emplace_back(m_Pool.Enqueue(std::move([code, chunkLocation]
					{
						code(chunkLocation);
					})));
				}
			}
		}

		for (auto& future : futures)
			future.wait();
	};

	loop(World::RENDER_DISTANCE, [&](const ChunkLocation& chunkLocation)
	{
		if (m_Chunks.find(chunkLocation) == m_Chunks.end())
		{
			Chunk chunk;
			chunk.Generate();
			SetChunk(chunkLocation, std::move(chunk));
		}
	});

	loop(World::RENDER_DISTANCE, [&](const ChunkLocation& chunkLocation)
	{
		Chunk* chunk = GetChunk(chunkLocation);

		if (chunk)
			chunk->GenerateMesh(chunkLocation);
	});
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

Block* World::GetHighestBlock(int x, int z)
{
	Chunk* chunk = GetChunk(PosUtils::ConvertWorldPosToChunkLoc(x, z));

	ChunkPosition chunkPosition = PosUtils::ConvertWorldPosToChunkPos({ x, 0, z });
	return (chunk) ? chunk->GetHighestBlock(chunkPosition.x, chunkPosition.z) : nullptr;
}

const Block* World::GetHighestBlock(int x, int z) const
{
	const Chunk* chunk = GetChunk(PosUtils::ConvertWorldPosToChunkLoc(x, z));

	ChunkPosition chunkPosition = PosUtils::ConvertWorldPosToChunkPos({ x, 0, z });
	return (chunk) ? chunk->GetHighestBlock(chunkPosition.x, chunkPosition.z) : nullptr;
}

std::optional<uint8_t> World::GetHighestBlockYPos(int x, int z) const
{
	const Chunk* chunk = GetChunk(PosUtils::ConvertWorldPosToChunkLoc(x, z));

	ChunkPosition chunkPosition = PosUtils::ConvertWorldPosToChunkPos({ x, 0, z });
	return (chunk) ? chunk->GetHighestBlockYPos(chunkPosition.x, chunkPosition.z) : std::nullopt;
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
