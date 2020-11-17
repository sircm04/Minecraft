#pragma once

#include "Entity.h"
#include "Chunk.h"

class Entity;
class Player;

enum class QueueState : uint8_t
{
	Unqueued,
	Queued
};

class World
{
private:
	std::unordered_map<glm::ivec2, Chunk> m_Chunks;
	std::vector<std::unique_ptr<Entity>> m_Entities;
	siv::PerlinNoise m_Noise;

public:
	bool m_FirstLoad = true;
	QueueState m_QueueState;

	World();

	void Update(Player* player, const glm::vec3& playerPosition);
	void RenderChunks();
	void RenderEntities();

	void UpdateChunks(Player* player, const glm::ivec2& playerChunkPosition);

	glm::ivec2 GetChunkPositionFromBlock(const glm::ivec2& position) const;
	glm::ivec3 GetBlockPositionInChunk(const glm::ivec3& position) const;

	void SetChunk(const glm::ivec2& position, Chunk&& chunk);
	Chunk* GetChunk(const glm::ivec2& position);
	const Chunk* GetChunk(const glm::ivec2& position) const;
	
	std::unordered_map<Chunk*, glm::ivec2> GetNeighboringChunks(const glm::ivec3& position);
	void RefreshNeighboringChunks(const glm::ivec3& position);

	bool SetBlock(const glm::ivec3& position, const Block& block);
	Block* GetBlock(const glm::ivec3& position);
	const Block* GetBlock(const glm::ivec3& position) const;

	int GetHighestBlockYPosition(const glm::ivec2& position) const;

	std::optional<glm::ivec3> GetTargetBlockPosition(glm::vec3 position, const glm::vec3& direction, int max, bool place = false) const;

	template<class Entity>
	void AddEntity(const Entity& entity);
};