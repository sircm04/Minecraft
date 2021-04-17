#include "../pch.h"
#include "World.h"

#include "../Entity/Mob/Player.h"
#include "../Entity/Mob/Cow.h"
#include "../Utils/Timer.h"

struct Job
{
	std::function<void(const glm::ivec2&)> job;
	const glm::ivec2& chunkPosition;
};

std::vector<Job> m_Jobs;
std::vector<std::thread> m_Threads;

World::World() noexcept
{
	std::random_device rand;
	m_NoiseRandom = std::mt19937(rand());
	m_Noise = siv::PerlinNoise(m_NoiseRandom);

	for (int i = 0; i < 32; ++i)
	{
		m_Threads.push_back(std::thread([&]()
		{
			while (true)
			{
				std::unique_lock lock(m_JobsMutex);

				if (!m_Jobs.empty()) {
					auto job = m_Jobs.back();
					m_Jobs.pop_back();
					std::cout << job.chunkPosition.x << ", " << job.chunkPosition.y << std::endl;
					lock.unlock();
					job.job(job.chunkPosition);
				}
			}
		}));
	}
}

World::~World() noexcept
{
	for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it)
		it->join();
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
	std::lock_guard lock(m_MainMutexLock);

	static constexpr unsigned int worldOuterRadius2 = WORLD_OUTER_RADIUS * WORLD_OUTER_RADIUS,
		worldRadius2 = WORLD_RADIUS * WORLD_RADIUS;

	for (auto it = m_Chunks.begin(); it != m_Chunks.end(); ++it)
		if (ceil(glm::distance2(static_cast<glm::vec2>(it->first), static_cast<glm::vec2>(playerChunkPosition))) >= worldOuterRadius2)
			it->second.SetRemoved();

	auto loop = [&](unsigned int radius, unsigned int radius2, auto&& code)
	{
		Timer timer("World::UpdateChunks subloop");
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
					std::scoped_lock lk(m_JobsMutex);

					m_Jobs.push_back({ code, chunkPosition });
				}
			}
		}

		while (true) {
			std::scoped_lock lk(m_JobsMutex);
			if (m_Jobs.empty()) break;
		}
	};

	loop(WORLD_OUTER_RADIUS, worldOuterRadius2, [&](const glm::ivec2& chunkPosition)
	{
		if (!GetChunk(chunkPosition))
		{
			Chunk chunk;
			chunk.Generate(&m_Noise, chunkPosition);
			SetChunk(chunkPosition, std::move(chunk));
		}
	});

	loop(WORLD_RADIUS, worldRadius2, [&](const glm::ivec2& chunkPosition)
	{
		Chunk* chunk = GetChunk(chunkPosition);

		if (chunk && *chunk->GetChunkState() == ChunkState::Generated)
		{
			chunk->GenerateTrees(this, chunkPosition);
			RefreshNeighboringChunks(chunkPosition);
		}
	});

	if (m_FirstLoad)
	{
		glm::ivec2 playerPosition = floor(glm::vec2 { player->m_Position.x, player->m_Position.z });
		uint8_t y = GetHighestBlockYPosition(playerPosition);
		if (y == -1)
			player->m_IsFlying = true;
		player->m_Position.x += 8.5f;
		player->m_Position.y = (y + 1.0f + abs(Player::PLAYER_AABB.GetMinimum().y));
		player->m_Position.z += 8.5f;
		AddEntity<Cow>(this, player->m_Position);
	}

	loop(WORLD_RADIUS, worldRadius2, [&](const glm::ivec2& chunkPosition)
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
	std::lock_guard lock(m_MutexLock);
	m_Chunks.emplace(position, std::move(chunk));
}

Chunk* World::GetChunk(const glm::ivec2& position) noexcept
{
	std::lock_guard lock(m_MutexLock);
	const auto& found = m_Chunks.find(position);
	return (found == m_Chunks.end()) ? nullptr : &found->second;
}

const Chunk* World::GetChunk(const glm::ivec2& position) const noexcept
{
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
	const glm::uvec3 inChunkPosition = GetBlockPositionInChunk(position);

	bool xAtMin = (inChunkPosition.x) == 0, xAtMax = (Chunk::CHUNK_WIDTH - 1),
		zAtMin = (inChunkPosition.z) == 0, zAtMax = (Chunk::CHUNK_DEPTH - 1);

	constexpr glm::ivec3 offsets[] = {
		{-1,  0, 0b01 }, { 0, -1, 0b10 },
		{ 1,  0, 0b01 }, { 0,  1, 0b10 },
		{-1, -1, 0b11 }, { 1,  1, 0b11 },
		{-1,  1, 0b11 }, { 1, -1, 0b11 }
	};

	for (auto offset : offsets)
	{
		auto target = chunkPosition + glm::ivec2 { offset };

		if ((!(offset.z & 1) || xAtMax) && (!(offset.z & 2) || zAtMax))
			chunks.emplace(GetChunk(target), target);
	}
	
	return chunks;
}

std::unordered_map<Chunk*, glm::ivec2> World::GetNeighboringChunks(const glm::ivec2& position) noexcept
{
	std::unordered_map<Chunk*, glm::ivec2> chunks;

	constexpr glm::ivec2 offsets[] = {
		{ -1,  0 }, { 0, -1 },
		{  1,  0 }, { 0,  1 },
		{ -1, -1 }, { 1,  1 },
		{ -1,  1 }, { 1, -1 }
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
