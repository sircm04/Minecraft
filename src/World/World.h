#pragma once

#include "../Entity/Entity.h"
#include "Chunk/Chunk.h"
#include "../Utils/ThreadPool.h"

class Entity;
class Player;

class World
{
public:
	static constexpr uint16_t WORLD_RADIUS = 24, WORLD_OUTER_RADIUS = WORLD_RADIUS + 1,
		REAL_WORLD_RADIUS = WORLD_RADIUS * 16;

private:
	std::unordered_map<glm::ivec2, Chunk> m_Chunks;
	std::vector<std::unique_ptr<Entity>> m_Entities;
	siv::PerlinNoise m_Noise;
	std::mt19937 m_NoiseRandom;

	mutable std::mutex m_Mutex, m_UpdateChunksMutex;

public:
	ThreadPool m_Pool;

	bool m_FirstLoad = true;

	World() noexcept;

	void Update(double deltaTime, Player* player, const glm::vec3& playerPosition);
	void RenderChunks(const ViewFrustum& frustum);
	void RenderEntities();

	void UpdateChunks(Player* player, const glm::ivec2& playerChunkPosition);
	void UpdateEntities(double deltaTime);

	static const glm::ivec2 GetChunkPositionFromBlock(const glm::ivec2& position) noexcept;
	static const glm::uvec3 GetBlockPositionInChunk(const glm::ivec3& position) noexcept;

	void SetChunk(const glm::ivec2& position, Chunk&& chunk) noexcept;
	Chunk* GetChunk(const glm::ivec2& position) noexcept;
	const Chunk* GetChunk(const glm::ivec2& position) const noexcept;
	const bool IsChunkLoaded(const glm::ivec2& position) const noexcept;
	
	std::unordered_map<Chunk*, glm::ivec2> GetNeighboringChunks(const glm::ivec3& position) noexcept;
	std::unordered_map<Chunk*, glm::ivec2> GetNeighboringChunks(const glm::ivec2& position) noexcept;
	void RefreshNeighboringChunks(const glm::ivec3& position) noexcept;
	void RefreshNeighboringChunks(const glm::ivec2& position) noexcept;

	void SetBlock(const glm::ivec3& position, const Block& block) noexcept;
	Block* GetBlock(const glm::ivec3& position) noexcept;
	const Block* GetBlock(const glm::ivec3& position) const noexcept;
	Block* GetBlockInBounds(const glm::ivec3& position) noexcept;
	const Block* GetBlockInBounds(const glm::ivec3& position) const noexcept;

	uint8_t GetHighestBlockYPosition(const glm::ivec2& position) const noexcept;

	const std::optional<glm::ivec3> GetTargetBlockPosition(glm::vec3 position, const glm::vec3& direction, int max, bool place = false) const noexcept;

	template<class E, typename... Ts>
	void AddEntity(Ts&&... vals);
};
