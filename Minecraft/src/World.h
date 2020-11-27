#pragma once

#include <vector>

#include "Entity.h"
#include "Chunk.h"

class Entity;
class Player;

const enum class QueueState : uint8_t
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

	std::unique_ptr<std::mutex> m_MutexLock;

public:
	static constexpr unsigned int WORLD_RADIUS = 24, WORLD_OUTER_RADIUS = WORLD_RADIUS + 1,
		REAL_WORLD_RADIUS = WORLD_RADIUS * 16;

	bool m_FirstLoad = true;
	QueueState m_QueueState;

	World() noexcept;

	void Update(double deltaTime, Player* player, const glm::vec3& playerPosition);
	void RenderChunks(const ViewFrustum& frustum, const glm::vec3& playerPosition);
	void RenderEntities();

	void UpdateChunks(Player* player, const glm::ivec2& playerChunkPosition);
	void UpdateEntities(double deltaTime);

	static const glm::ivec2 GetChunkPositionFromBlock(const glm::ivec2& position) noexcept;
	static const glm::ivec3 GetBlockPositionInChunk(const glm::ivec3& position) noexcept;

	void SetChunk(const glm::ivec2& position, Chunk&& chunk) noexcept;
	Chunk* GetChunk(const glm::ivec2& position) noexcept;
	const Chunk* GetChunk(const glm::ivec2& position) const noexcept;
	
	std::unordered_map<Chunk*, glm::ivec2> GetNeighboringChunks(const glm::ivec3& position) noexcept;
	void RefreshNeighboringChunks(const glm::ivec3& position) noexcept;

	bool SetBlock(const glm::ivec3& position, const Block& block) noexcept;
	Block* GetBlock(const glm::ivec3& position) noexcept;
	const Block* GetBlock(const glm::ivec3& position) const noexcept;

	int GetHighestBlockYPosition(const glm::ivec2& position) const noexcept;

	const std::optional<glm::ivec3> GetTargetBlockPosition(glm::vec3 position, const glm::vec3& direction, int max, bool place = false) const noexcept;

	template<class Entity>
	void AddEntity(const Entity& entity);
};