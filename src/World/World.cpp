#include "../pch.h"
#include "World.h"

#include "../Entity/Mob/Player.h"
#include "../Entity/Mob/Cow.h"

#include "../Utils/Timer.h"

World::World() noexcept
{
	std::random_device rand;
	m_NoiseRandom = std::mt19937(rand());
	m_Noise = siv::PerlinNoise(m_NoiseRandom);
}

void World::Update(double deltaTime, Player* player, const glm::vec3& playerPosition)
{
	static glm::ivec2 previousPlayerChunkPosition;
	const glm::ivec2 playerChunkPosition = GetChunkPositionFromBlock(floor(glm::vec2 { playerPosition.x, playerPosition.z }));

	if (playerChunkPosition != previousPlayerChunkPosition || m_FirstLoad)
		UpdateChunks(player, playerChunkPosition);

	previousPlayerChunkPosition = playerChunkPosition;

	UpdateEntities(deltaTime);
}

void World::RenderChunks(const ViewFrustum& frustum)
{
	auto it = m_Chunks.begin();
	while (it != m_Chunks.end())
	{
		if (*it->second.GetChunkState() == ChunkState::Removed)
		{
			it = m_Chunks.erase(it);
		}
		else
		{
			if (it->second.m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Generated)
				it->second.m_ChunkMesh.BufferMesh();

			if (it->second.m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Complete)
			{
				it->second.m_ChunkMesh.Bind();
				it->second.m_ChunkMesh.Render(frustum, it->first);
			}

			++it;
		}
	}
}

void World::RenderEntities()
{
	for (const auto& entity : m_Entities)
		entity->Render();
}

void World::UpdateChunks(Player* player, const glm::ivec2& playerChunkPosition)
{
	static constexpr unsigned int worldOuterRadius2 = WORLD_OUTER_RADIUS * WORLD_OUTER_RADIUS;
	for (auto it = m_Chunks.begin(); it != m_Chunks.end(); ++it)
		if (ceil(glm::distance2(static_cast<glm::vec2>(it->first), static_cast<glm::vec2>(playerChunkPosition))) >= worldOuterRadius2)
			it->second.SetRemoved();

	auto loop = [&](unsigned int radius, auto&& code)
	{
		Timer timer("World::UpdateChunks subloop");
		std::vector<std::future<void>> futures;

		int xPositiveDistance = radius + playerChunkPosition.x,
			zPositiveDistance = radius + playerChunkPosition.y,
			xNegativeDistance = -radius + playerChunkPosition.x,
			zNegativeDistance = -radius + playerChunkPosition.y;

		const float radius2 = static_cast<float>(radius * radius);

		for (int x = xNegativeDistance; x < xPositiveDistance; ++x)
			futures.push_back(std::async(std::launch::async, code, x, zNegativeDistance, zPositiveDistance, radius2, playerChunkPosition));
	};

	loop(WORLD_OUTER_RADIUS, [&](int x, int zNegativeDistance, int zPositiveDistance, int radius2, const glm::ivec2& playerChunkPosition) {
		for (int z = zNegativeDistance; z < zPositiveDistance; ++z)
		{
			const glm::ivec2 chunkPosition = { x, z };

			if (ceil(glm::distance2(static_cast<glm::vec2>(chunkPosition), static_cast<glm::vec2>(playerChunkPosition))) < radius2)
			{
				if (!GetChunk(chunkPosition))
				{
					Chunk chunk;
					chunk.Generate(&m_Noise, chunkPosition);
					SetChunk(chunkPosition, std::move(chunk));
				}
			}
		}
	});

	if (m_FirstLoad)
	{
		m_FirstLoad = false;
		int y = GetHighestBlockYPosition(floor(glm::vec2 { player->m_Position.x, player->m_Position.z }));
		if (y != -1)
		{
			player->m_Position.x += 0.5f;
			player->m_Position.y = (y + 2.5f);
			player->m_Position.z += 0.5f;
			AddEntity<Cow>(this, player->m_Position);
		}
	}

	loop(WORLD_RADIUS, [&](int x, int zNegativeDistance, int zPositiveDistance, int radius2, const glm::ivec2& playerChunkPosition) {
		for (int z = zNegativeDistance; z < zPositiveDistance; ++z)
		{
			const glm::ivec2 chunkPosition = { x, z };

			if (ceil(glm::distance2(static_cast<glm::vec2>(chunkPosition), static_cast<glm::vec2>(playerChunkPosition))) < radius2)
			{
				Chunk* chunk = GetChunk(chunkPosition);

				if (chunk && *chunk->GetChunkState() == ChunkState::Generated
					&& chunk->m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Ungenerated)
					chunk->GenerateMesh(this, chunkPosition);
			}
		}
	});
}

void World::UpdateEntities(double deltaTime)
{
	for (auto& entity : m_Entities)
		entity->Update(deltaTime);
}

const glm::ivec2 World::GetChunkPositionFromBlock(const glm::ivec2& position) noexcept
{
	return {
		(position.x >> 4),
		(position.y >> 4)
	};
}

const glm::uvec3 World::GetBlockPositionInChunk(const glm::ivec3& position) noexcept
{
	return {
		(position.x & 15),
		position.y,
		(position.z & 15)
	};
}

void World::SetChunk(const glm::ivec2& position, Chunk&& chunk) noexcept
{
	std::lock_guard lock(m_MutexLock);
	m_Chunks.emplace(position, std::move(chunk));
}

Chunk* World::GetChunk(const glm::ivec2& position) noexcept
{
	std::lock_guard lock(m_MutexLock);
	auto found = m_Chunks.find(position);
	return (found == m_Chunks.end()) ? nullptr : &found->second;
}

const Chunk* World::GetChunk(const glm::ivec2& position) const noexcept
{
	std::lock_guard lock(m_MutexLock);
	const auto& found = m_Chunks.find(position);
	return (found == m_Chunks.end()) ? nullptr : &found->second;
}

std::unordered_map<Chunk*, glm::ivec2> World::GetNeighboringChunks(const glm::ivec3& position) noexcept
{
	std::unordered_map<Chunk*, glm::ivec2> chunks;

	const glm::ivec2 chunkPosition = GetChunkPositionFromBlock({ position.x, position.z });
	const glm::uvec3 inChunkPosition = GetBlockPositionInChunk(position);

	auto emplaceif = [&](bool boolean, glm::ivec2 chunkPos)
	{
		if (boolean)
			chunks.emplace(GetChunk(chunkPos), chunkPos);
	};

	bool xAtMin = (inChunkPosition.x) == 0, xAtMax = (Chunk::CHUNK_WIDTH - 1),
		zAtMin = (inChunkPosition.z) == 0, zAtMax = (Chunk::CHUNK_DEPTH - 1);

	emplaceif(xAtMin, { chunkPosition.x - 1, chunkPosition.y });
	emplaceif(zAtMin, { chunkPosition.x, chunkPosition.y - 1 });
	emplaceif(xAtMax, { chunkPosition.x + 1, chunkPosition.y });
	emplaceif(zAtMax, { chunkPosition.x, chunkPosition.y + 1 });
	emplaceif(xAtMin && zAtMin, { chunkPosition.x - 1, chunkPosition.y - 1 });
	emplaceif(xAtMax && zAtMax, { chunkPosition.x + 1, chunkPosition.y + 1 });
	emplaceif(xAtMin && zAtMax, { chunkPosition.x - 1, chunkPosition.y + 1 });
	emplaceif(xAtMax && zAtMin, { chunkPosition.x + 1, chunkPosition.y - 1 });
	
	return chunks;
}

void World::RefreshNeighboringChunks(const glm::ivec3& position) noexcept
{
	std::unordered_map<Chunk*, glm::ivec2> neighboringChunks = GetNeighboringChunks(position);
	std::cout << "\n";
	for (auto it : neighboringChunks)
	{
		it.first->GenerateMesh(this, it.second);
		std::cout << it.second.x << ", " << it.second.y << "\n";
	}
}

void World::SetBlock(const glm::ivec3& position, const Block& block) noexcept
{
	Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));
	chunk->SetBlock(GetBlockPositionInChunk(position), block);
}

Block* World::GetBlock(const glm::ivec3& position) noexcept
{
	Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));
	return ((chunk) ? chunk->GetBlock(GetBlockPositionInChunk(position)) : nullptr);
}

const Block* World::GetBlock(const glm::ivec3& position) const noexcept
{
	const Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));
	return ((chunk) ? chunk->GetBlock(GetBlockPositionInChunk(position)) : nullptr);
}

int World::GetHighestBlockYPosition(const glm::ivec2& position) const noexcept
{
	const Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.y }));

	if (chunk)
	{
		for (uint8_t y = Chunk::CHUNK_HEIGHT; y > 0; --y)
		{
			const Block* block = chunk->GetBlock({ position.x, y, position.y });

			if (block && block->GetBlockTypeData().isSolid)
				return y;
		}
	}

	return -1;
}

const std::optional<glm::ivec3> World::GetTargetBlockPosition(glm::vec3 position, const glm::vec3& direction, int max, bool place) const noexcept
{
	glm::ivec3 blockPos;
	glm::vec3 sign;

	for (uint8_t i = 0; i < 3; ++i)
		sign[i] = direction[i] > 0;

	for (int i = 0; i < max; ++i) {
		glm::vec3 tvec = (floor(position + sign) - position) / direction;
		float t = std::min(tvec.x, std::min(tvec.y, tvec.z));

		position += direction * (t + 0.001f);

		const Block* block = GetBlock(blockPos = floor(glm::vec3 {
			position.x,
			position.y,
			position.z
		}));

		if (block && block->GetBlockTypeData().isSolid)
		{
			if (place)
			{
				glm::vec3 normal;
				for (uint8_t i = 0; i < 3; ++i) {
					normal[i] = (t == tvec[i]);
					if (sign[i]) normal[i] = -normal[i];
				}

				position += normal;

				blockPos = floor(glm::vec3 {
					position.x,
					position.y,
					position.z
				});
			}

			return blockPos;
		}
	}

	return std::nullopt;
}

template<class E, typename... Ts>
void World::AddEntity(Ts&&... vals)
{
	m_Entities.emplace_back(std::make_unique<E>(std::forward<Ts>(vals)...));
}