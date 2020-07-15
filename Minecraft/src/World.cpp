#include "pch.h"
#include "World.h"

#include "Cow.h"

#include <future>

static const unsigned int radius = 24, outerRadius = radius + 1;

glm::ivec2 previousPlayerChunkPosition = { -1, -1 };

World::World()
	: m_QueueState(QueueState::Unqueued)
{
	srand(time(0));
	m_Noise = siv::PerlinNoise(rand() % 1000);
}

void World::Update(glm::vec3* playerPosition)
{
	const glm::ivec2& playerChunkPosition = GetChunkPositionFromBlock({ floor(playerPosition->x), floor(playerPosition->z) });

	if (playerChunkPosition != previousPlayerChunkPosition)
		m_QueueState = QueueState::Queued;

	previousPlayerChunkPosition = playerChunkPosition;

	if (m_QueueState == QueueState::Queued)
	{
		m_QueueState = QueueState::Unqueued;

		UpdateChunks(playerPosition, playerChunkPosition);
	}
}

void World::RenderChunks()
{
	int i = 0;

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
				it->second.m_ChunkMesh.Render();
			}

			++i;
			++it;
		}
	}

	std::cout << "Iterated Chunks: " << i << ", m_Chunks size: " << m_Chunks.size() << std::endl;
}

void World::RenderEntities(Shader* shader)
{
	for (auto it = m_Entities.begin(); it != m_Entities.end(); ++it)
		(*it)->Render(shader);
}

void World::UpdateChunks(glm::vec3* playerPosition, const glm::ivec2& playerChunkPosition)
{
	for (auto it = m_Chunks.begin(); it != m_Chunks.end(); ++it)
	{
		if (m_QueueState == QueueState::Queued)
			return;

		if (glm::distance((glm::vec2) it->first, (glm::vec2) playerChunkPosition) >= outerRadius)
			it->second.SetRemoved();
	}

	int xPositiveDistance = outerRadius + playerChunkPosition.x,
		zPositiveDistance = outerRadius + playerChunkPosition.y,
		xNegativeDistance = -outerRadius + playerChunkPosition.x,
		zNegativeDistance = -outerRadius + playerChunkPosition.y;

	for (int x = xNegativeDistance; x < xPositiveDistance; ++x)
	{
		for (int z = zNegativeDistance; z < zPositiveDistance; ++z)
		{
			if (m_QueueState == QueueState::Queued)
				return;

			const glm::ivec2& chunkPosition = { x, z };
			const Chunk* chunk = GetChunk(chunkPosition);

			if (!chunk && glm::distance((glm::vec2) chunkPosition, (glm::vec2) playerChunkPosition) < outerRadius)
			{
				Chunk chunk = Chunk();
				chunk.Generate(&m_Noise, chunkPosition);
				SetChunk(chunkPosition, std::move(chunk));

				Chunk* leftChunk = GetChunk({ chunkPosition.x - 1, chunkPosition.y });
				Chunk* backChunk = GetChunk({ chunkPosition.x, chunkPosition.y - 1 });

				if (leftChunk && leftChunk->m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Complete)
					leftChunk->m_ChunkMesh.m_ChunkMeshState = ChunkMeshState::Ungenerated;

				if (backChunk && backChunk->m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Complete)
					backChunk->m_ChunkMesh.m_ChunkMeshState = ChunkMeshState::Ungenerated;
			}
		}
	}

	if (m_FirstLoad)
	{
		int y = GetHighestBlockYPosition({ playerPosition->x, playerPosition->z });
		if (y != -1)
		{
			playerPosition->y = (y + 2.5f);
			AddEntity<Cow>(Cow(*playerPosition));
		}
		m_FirstLoad = false;
	}

	xPositiveDistance = radius + playerChunkPosition.x;
	zPositiveDistance = radius + playerChunkPosition.y;
	xNegativeDistance = -radius + playerChunkPosition.x;
	zNegativeDistance = -radius + playerChunkPosition.y;

	for (int x = xNegativeDistance; x < xPositiveDistance; ++x)
	{
		for (int z = zNegativeDistance; z < zPositiveDistance; ++z)
		{
			if (m_QueueState == QueueState::Queued)
				return;

			const glm::ivec2& chunkPosition = { x, z };
			Chunk* chunk = GetChunk(chunkPosition);

			if (chunk && glm::distance((glm::vec2) chunkPosition, (glm::vec2) playerChunkPosition) < radius
				&& *chunk->GetChunkState() == ChunkState::Generated && chunk->m_ChunkMesh.m_ChunkMeshState == ChunkMeshState::Ungenerated)
				chunk->GenerateMesh(this, chunkPosition);
		}
	}
}

glm::ivec2 World::GetChunkPositionFromBlock(const glm::ivec2& position)
{
	return {
		(position.x >> Chunk::CHUNK_X_SHIFT),
		(position.y >> Chunk::CHUNK_Z_SHIFT)
	};
}

glm::ivec3 World::GetBlockPositionInChunk(const glm::ivec3& position)
{
	return {
		(position.x & Chunk::CHUNK_X_MASK),
		position.y,
		(position.z & Chunk::CHUNK_Z_MASK)
	};
}

void World::SetChunk(const glm::ivec2& position, Chunk&& chunk)
{
	m_Chunks.emplace(position, std::move(chunk));
}

Chunk* World::GetChunk(const glm::ivec2& position)
{
	auto found = m_Chunks.find(position);
	return (found == m_Chunks.end()) ? nullptr : &found->second;
}

std::unordered_map<Chunk*, glm::ivec2> World::GetNeighboringChunks(const glm::ivec3& position)
{
	std::unordered_map<Chunk*, glm::ivec2> chunks;

	const glm::ivec2& chunkPosition = GetChunkPositionFromBlock({ position.x, position.z });
	const glm::ivec3& inChunkPosition = GetBlockPositionInChunk(position);

	if (inChunkPosition.x == 0)
	{
		glm::ivec2 chunkPos = { chunkPosition.x - 1, chunkPosition.y };
		chunks.emplace(GetChunk(chunkPos), chunkPos);
	}

	if (inChunkPosition.z == 0)
	{
		glm::ivec2 chunkPos = { chunkPosition.x, chunkPosition.y - 1 };
		chunks.emplace(GetChunk(chunkPos), chunkPos);
	}

	if (inChunkPosition.x == Chunk::CHUNK_WIDTH)
	{
		glm::ivec2 chunkPos = { chunkPosition.x + 1, chunkPosition.y };
		chunks.emplace(GetChunk(chunkPos), chunkPos);
	}

	if (inChunkPosition.z == Chunk::CHUNK_DEPTH)
	{
		glm::ivec2 chunkPos = { chunkPosition.x, chunkPosition.y + 1 };
		chunks.emplace(GetChunk(chunkPos), chunkPos);
	}

	return chunks;
}

void World::RefreshNeighboringChunks(const glm::ivec3& position)
{
	std::unordered_map<Chunk*, glm::ivec2> neighboringChunks = GetNeighboringChunks(position);
	for (auto it = neighboringChunks.begin(); it != neighboringChunks.end(); ++it)
		it->first->GenerateMesh(this, it->second);
}

bool World::SetBlock(const glm::ivec3& position, const Block& block)
{
	Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));

	if (chunk)
		return chunk->SetBlock(GetBlockPositionInChunk(position), block);

	return false;
}

Block* World::GetBlock(const glm::ivec3& position)
{
	Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.z }));

	if (chunk)
		return chunk->GetBlock(GetBlockPositionInChunk(position));

	return nullptr;
}

int World::GetHighestBlockYPosition(const glm::ivec2& position)
{
	Chunk* chunk = GetChunk(GetChunkPositionFromBlock({ position.x, position.y }));

	if (chunk)
	{
		for (int y = Chunk::CHUNK_HEIGHT; y > 0; y--)
		{
			Block* block = chunk->GetBlock({ position.x, y, position.y });

			if (block && block->GetBlockTypeData()->isSolid)
				return y;
		}
	}

	return -1;
}

std::optional<glm::ivec3> World::GetTargetBlockPosition(glm::vec3 position, const glm::vec3& direction, int max, bool place)
{
	glm::ivec3 blockPos;
	glm::vec3 sign;

	for (int i = 0; i < 3; ++i)
		sign[i] = direction[i] > 0;

	for (int i = 0; i < max; ++i) {
		glm::vec3 tvec = (floor(position + sign) - position) / direction;
		float t = std::min(tvec.x, std::min(tvec.y, tvec.z));

		position += direction * (t + 0.001f);

		Block* block = GetBlock(blockPos = {
			floor(position.x),
			floor(position.y),
			floor(position.z)
		});

		if (block && block->GetBlockTypeData()->isSolid)
		{
			if (place)
			{
				glm::vec3 normal;
				for (int i = 0; i < 3; ++i) {
					normal[i] = (t == tvec[i]);
					if (sign[i]) normal[i] = -normal[i];
				}

				position += normal;

				blockPos = {
					floor(position.x),
					floor(position.y),
					floor(position.z)
				};
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