#include "pch.h"
#include "World.h"

#include "Player.h"
#include "Cow.h"

World::World() noexcept
	: m_MutexLock(std::make_unique<std::mutex>()), m_QueueState(QueueState::Unqueued)
{
	m_Noise = siv::PerlinNoise(std::mt19937{});
}

void World::Update(double deltaTime, Player* player, const glm::vec3& playerPosition)
{
	static glm::ivec2 previousPlayerChunkPosition = { -1, -1 };
	const glm::ivec2 playerChunkPosition = GetChunkPositionFromBlock(floor(glm::vec2 { playerPosition.x, playerPosition.z }));

	if (playerChunkPosition != previousPlayerChunkPosition)
		m_QueueState = QueueState::Queued;

	previousPlayerChunkPosition = playerChunkPosition;

	if (m_QueueState == QueueState::Queued)
	{
		m_QueueState = QueueState::Unqueued;

		UpdateChunks(player, playerChunkPosition);
	}

	UpdateEntities(deltaTime);
}

void World::RenderChunks(const ViewFrustum& frustum, const glm::vec3& playerPosition)
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
				it->second.m_ChunkMesh.Render(frustum, it->first, playerPosition);
			}

			++it;
		}
	}
}

void World::RenderEntities()
{
	for (auto it = m_Entities.begin(); it != m_Entities.end(); ++it)
		(*it)->Render();
}

void World::UpdateChunks(Player* player, const glm::ivec2& playerChunkPosition)
{
	for (auto it = m_Chunks.begin(); it != m_Chunks.end(); ++it)
	{
		if (m_QueueState == QueueState::Queued)
			return;

		if (ceil(glm::distance(static_cast<glm::vec2>(it->first), static_cast<glm::vec2>(playerChunkPosition))) >= WORLD_OUTER_RADIUS)
			it->second.SetRemoved();
	}

	auto loop = [&](unsigned int radius, auto&& code)
	{
		std::vector<std::future<void>> futures;

		int xPositiveDistance = radius + playerChunkPosition.x,
			zPositiveDistance = radius + playerChunkPosition.y,
			xNegativeDistance = -radius + playerChunkPosition.x,
			zNegativeDistance = -radius + playerChunkPosition.y;

		for (int x = xNegativeDistance; x < xPositiveDistance; ++x)
		{
			for (int z = zNegativeDistance; z < zPositiveDistance; ++z)
			{
				if (m_QueueState == QueueState::Queued)
				{
					for (auto& future : futures)
						future.wait();
					return;
				}

				const glm::ivec2 chunkPosition = { x, z };

				if (ceil(glm::distance(static_cast<glm::vec2>(chunkPosition), static_cast<glm::vec2>(playerChunkPosition))) < radius)
					futures.push_back(std::async(std::launch::async, code, x, z, chunkPosition, playerChunkPosition));
			}
		}

		for (auto& future : futures)
			future.wait();
	};

	loop(WORLD_OUTER_RADIUS, [&](int x, int z, const glm::ivec2& chunkPosition, const glm::ivec2& playerChunkPosition) {
		if (!GetChunk(chunkPosition))
		{
			Chunk chunk;
			chunk.Generate(&m_Noise, chunkPosition);
			SetChunk(chunkPosition, std::move(chunk));

			/*Chunk* leftChunk = GetChunk({ chunkPosition.x - 1, chunkPosition.y });
			Chunk* backChunk = GetChunk({ chunkPosition.x, chunkPosition.y - 1 });

			if (leftChunk && leftChunk->m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Complete)
				leftChunk->m_ChunkMesh.m_ChunkMeshState = ChunkMeshState::Ungenerated;

			if (backChunk && backChunk->m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Complete)
				backChunk->m_ChunkMesh.m_ChunkMeshState = ChunkMeshState::Ungenerated;*/
		}
	});
	
	if (m_FirstLoad)
	{
		m_FirstLoad = false;
		int y = GetHighestBlockYPosition(floor(glm::vec2 { player->m_Position.x, player->m_Position.z }));
		if (y != -1)
		{
			player->m_Position.y = (y + 2.5f);
			//AddEntity<Cow>(Cow(this, player->m_Position));
		}
	}

	loop(WORLD_RADIUS, [&](int x, int z, const glm::ivec2& chunkPosition, const glm::ivec2& playerChunkPosition) {
		Chunk* chunk = GetChunk(chunkPosition);

		if (chunk && *chunk->GetChunkState() == ChunkState::Generated
			&& chunk->m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Ungenerated)
			chunk->GenerateMesh(this, chunkPosition);
	});
}

void World::UpdateEntities(double deltaTime)
{
	for (auto it = m_Entities.begin(); it != m_Entities.end(); ++it)
		(*it)->Update(deltaTime);
}

const glm::ivec2 World::GetChunkPositionFromBlock(const glm::ivec2& position) noexcept
{
	return {
		(position.x >> Chunk::CHUNK_X_SHIFT),
		(position.y >> Chunk::CHUNK_Z_SHIFT)
	};
}

const glm::ivec3 World::GetBlockPositionInChunk(const glm::ivec3& position) noexcept
{
	return {
		(position.x & Chunk::CHUNK_X_MASK),
		position.y,
		(position.z & Chunk::CHUNK_Z_MASK)
	};
}

void World::SetChunk(const glm::ivec2& position, Chunk&& chunk) noexcept
{
	std::lock_guard lock(*m_MutexLock);
	m_Chunks.emplace(position, std::move(chunk));
}

Chunk* World::GetChunk(const glm::ivec2& position) noexcept
{
	std::lock_guard lock(*m_MutexLock);
	auto& found = m_Chunks.find(position);
	return (found == m_Chunks.end()) ? nullptr : &found->second;
}

const Chunk* World::GetChunk(const glm::ivec2& position) const noexcept
{
	std::lock_guard lock(*m_MutexLock);
	const auto& found = m_Chunks.find(position);
	return (found == m_Chunks.end()) ? nullptr : &found->second;
}

std::unordered_map<Chunk*, glm::ivec2> World::GetNeighboringChunks(const glm::ivec3& position) noexcept
{
	std::unordered_map<Chunk*, glm::ivec2> chunks;

	const glm::ivec2 chunkPosition = GetChunkPositionFromBlock({ position.x, position.z });
	const glm::ivec3 inChunkPosition = GetBlockPositionInChunk(position);

	auto test = [&](bool boolean, glm::ivec2 chunkPos)
	{
		if (boolean)
			chunks.emplace(GetChunk(chunkPos), chunkPos);
	};
	
	test(inChunkPosition.x == 0, { chunkPosition.x - 1, chunkPosition.y });
	test(inChunkPosition.z == 0, { chunkPosition.x, chunkPosition.y - 1 });
	test(inChunkPosition.x == Chunk::CHUNK_WIDTH, { chunkPosition.x + 1, chunkPosition.y });
	test(inChunkPosition.z == Chunk::CHUNK_DEPTH, { chunkPosition.x, chunkPosition.y + 1});
	
	return chunks;
}

void World::RefreshNeighboringChunks(const glm::ivec3& position) noexcept
{
	std::unordered_map<Chunk*, glm::ivec2> neighboringChunks = GetNeighboringChunks(position);
	for (auto it = neighboringChunks.begin(); it != neighboringChunks.end(); ++it)
		it->first->GenerateMesh(this, it->second);
}

bool World::SetBlock(const glm::ivec3& position, const Block& block) noexcept
{
	Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));

	if (chunk)
		return chunk->SetBlock(GetBlockPositionInChunk(position), block);

	return false;
}

Block* World::GetBlock(const glm::ivec3& position) noexcept
{
	Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));

	if (chunk)
		return chunk->GetBlock(GetBlockPositionInChunk(position));

	return nullptr;
}

const Block* World::GetBlock(const glm::ivec3& position) const noexcept
{
	const Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));

	if (chunk)
		return chunk->GetBlock(GetBlockPositionInChunk(position));

	return nullptr;
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
				for (int i = 0; i < 3; ++i) {
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

template<class Entity>
void World::AddEntity(const Entity& entity)
{
	m_Entities.emplace_back(std::make_unique<Entity>(entity));
}