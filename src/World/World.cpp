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
			std::lock_guard lock(m_Mutex);
			it = m_Chunks.erase(it);
			continue;
		}

		if (it->second.m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Generated)
			it->second.m_ChunkMesh.BufferMesh();

		if (it->second.m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Complete)
			it->second.m_ChunkMesh.Render(frustum, it->first);

		++it;
	}
}

void World::RenderEntities()
{
	for (const auto& entity : m_Entities)
		entity->Render();
}

void World::UpdateChunks(Player* player, const glm::ivec2& playerChunkPosition)
{
	std::lock_guard lock(m_UpdateChunksMutex);

	static constexpr unsigned int worldOuterRadius2 = WORLD_OUTER_RADIUS * WORLD_OUTER_RADIUS,
		worldRadius2 = WORLD_RADIUS * WORLD_RADIUS;

	for (auto it = m_Chunks.begin(); it != m_Chunks.end(); ++it)
		if (ceil(glm::distance2(static_cast<glm::vec2>(it->first), static_cast<glm::vec2>(playerChunkPosition))) >= worldOuterRadius2)
			it->second.SetRemoved();

	auto loop = [&](const std::string& action, unsigned int radius, unsigned int radius2, auto code)
	{
		Timer timer(action.c_str());

		std::vector<std::future<void>> futures;

		int xPositiveDistance = radius + playerChunkPosition.x,
			zPositiveDistance = radius + playerChunkPosition.y,
			xNegativeDistance = -radius + playerChunkPosition.x,
			zNegativeDistance = -radius + playerChunkPosition.y;

		for (int x = xNegativeDistance; x < xPositiveDistance; ++x)
		{
			for (int z = zNegativeDistance; z < zPositiveDistance; ++z)
			{
				const glm::ivec2 chunkPosition = { x, z };

				if (ceil(glm::distance2(static_cast<glm::vec2>(chunkPosition), static_cast<glm::vec2>(playerChunkPosition))) < radius2)
				{
					futures.emplace_back(m_Pool.Enqueue(std::move([code, chunkPosition]
					{
						code(chunkPosition);
					})));
				}
			}
		}

		for (auto& future : futures)
			future.wait();
	};

	loop("Generation", WORLD_OUTER_RADIUS, worldOuterRadius2, [&](const glm::ivec2& chunkPosition)
	{
		if (!GetChunk(chunkPosition))
		{
			Chunk chunk;
			chunk.Generate(&m_Noise, chunkPosition);
			SetChunk(chunkPosition, std::move(chunk));
		}
	});

	//loop("Tree Generation", WORLD_RADIUS, worldRadius2, [&](const glm::ivec2& chunkPosition)
	//{
	//	Chunk* chunk = GetChunk(chunkPosition);

	//	if (chunk && *chunk->GetChunkState() == ChunkState::Generated)
	//	{
	//		chunk->GenerateTrees(this, chunkPosition);
	//		RefreshNeighboringChunks(chunkPosition);
	//	}
	//});

	if (m_FirstLoad)
	{
		player->m_Position.x += 8.5f;
		player->m_Position.z += 8.5f;
		uint8_t y = GetHighestBlockYPosition(floor(glm::vec2{ player->m_Position.x, player->m_Position.z }));
		if (y == -1)
			player->m_IsFlying = true;
		player->m_Position.y = (y + 1.0f + abs(Player::PLAYER_AABB.GetMinimum().y));
		AddEntity<Cow>(this, player->m_Position);
	}

	loop("Meshing", WORLD_RADIUS, worldRadius2, [&](const glm::ivec2& chunkPosition)
	{
		Chunk* chunk = GetChunk(chunkPosition);
		
		if (chunk && *chunk->GetChunkState() == ChunkState::Complete
			&& chunk->m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Ungenerated)
			chunk->GenerateMesh(this, chunkPosition);
	});

	if (m_FirstLoad)
		m_FirstLoad = false;
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
	std::lock_guard lock(m_Mutex);
	m_Chunks.emplace(position, std::move(chunk));
}

Chunk* World::GetChunk(const glm::ivec2& position) noexcept
{
	std::lock_guard lock(m_Mutex);
	const auto& found = m_Chunks.find(position);
	return (found == m_Chunks.end()) ? nullptr : &found->second;
}

const Chunk* World::GetChunk(const glm::ivec2& position) const noexcept
{
	std::lock_guard lock(m_Mutex);
	const auto& found = m_Chunks.find(position);
	return (found == m_Chunks.end()) ? nullptr : &found->second;
}

const bool World::IsChunkLoaded(const glm::ivec2& position) const noexcept
{
	const auto& found = m_Chunks.find(position);
	return found != m_Chunks.end() && *found->second.GetChunkState() >= ChunkState::Generated;
}

std::unordered_map<Chunk*, glm::ivec2> World::GetNeighboringChunks(const glm::ivec3& position) noexcept
{
	std::unordered_map<Chunk*, glm::ivec2> chunks;

	const glm::ivec2 chunkPosition = GetChunkPositionFromBlock({ position.x, position.z });
	const glm::ivec3 inChunkPosition = GetBlockPositionInChunk(position);

	auto emplaceChunk = [&](bool boolean, const glm::ivec2 chunkPos)
	{
		if (boolean)
			chunks.emplace(GetChunk(chunkPos), chunkPos);
	};

	emplaceChunk(inChunkPosition.x == 0, { chunkPosition.x - 1, chunkPosition.y });
	emplaceChunk(inChunkPosition.z == 0, { chunkPosition.x, chunkPosition.y - 1 });
	emplaceChunk(inChunkPosition.x == Chunk::CHUNK_WIDTH, { chunkPosition.x + 1, chunkPosition.y });
	emplaceChunk(inChunkPosition.z == Chunk::CHUNK_DEPTH, { chunkPosition.x, chunkPosition.y + 1 });

	return chunks;
}

std::unordered_map<Chunk*, glm::ivec2> World::GetNeighboringChunks(const glm::ivec2& position) noexcept
{
	std::unordered_map<Chunk*, glm::ivec2> chunks;

	constexpr glm::ivec2 offsets[] = {
		{ -1, 0 },
		{ 1, 0 },
		{ 0, -1 },
		{ 0, 1 }
	};

	for (auto offset : offsets)
	{
		auto target = position + offset;
		chunks.emplace(GetChunk(target), target);
	}

	return chunks;
}

void World::RefreshNeighboringChunks(const glm::ivec3& position) noexcept
{
	std::unordered_map<Chunk*, glm::ivec2> neighboringChunks = GetNeighboringChunks(position);
	for (auto element : neighboringChunks)
		if (element.first && element.first->m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Complete)
			element.first->GenerateMesh(this, element.second);
}

void World::RefreshNeighboringChunks(const glm::ivec2& position) noexcept
{
	std::unordered_map<Chunk*, glm::ivec2> neighboringChunks = GetNeighboringChunks(position);
	for (auto element : neighboringChunks)
		if (element.first && element.first->m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Complete)
			element.first->GenerateMesh(this, element.second);
}

void World::SetBlock(const glm::ivec3& position, const Block& block) noexcept
{
	Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));
	if (chunk)
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

Block* World::GetBlockInBounds(const glm::ivec3& position) noexcept
{
	Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));
	return ((chunk) ? chunk->GetBlockInBounds(GetBlockPositionInChunk(position)) : nullptr);
}

const Block* World::GetBlockInBounds(const glm::ivec3& position) const noexcept
{
	const Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));
	return ((chunk) ? chunk->GetBlockInBounds(GetBlockPositionInChunk(position)) : nullptr);
}

uint8_t World::GetHighestBlockYPosition(const glm::ivec2& position) const noexcept
{
	const Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.y }));

	if (chunk)
		return chunk->GetHighestBlockYPosition(position);

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

		const Block* block = GetBlockInBounds(blockPos = floor(glm::vec3 {
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
