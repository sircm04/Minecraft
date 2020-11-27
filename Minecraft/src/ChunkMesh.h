#pragma once

#include <mutex>

#include "Mesh.h"
#include "Block.h"

#include "Frustum.h"

class World;
class Chunk;

static constexpr float FRONT_BLOCK_FACE_VERTICES[] = {
	// front
	0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f
};

static constexpr float BACK_BLOCK_FACE_VERTICES[] {
	// back
	1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
};

static constexpr float LEFT_BLOCK_FACE_VERTICES[] {
	// left
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f
};

static constexpr float RIGHT_BLOCK_FACE_VERTICES[] {
	// right
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f
};

static constexpr float TOP_BLOCK_FACE_VERTICES[] {
	// top
	1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
};

static constexpr float BOTTOM_BLOCK_FACE_VERTICES[] {
	// bottom
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f
};

const enum class ChunkMeshState : uint8_t
{
	Ungenerated,
	Generated,
	Complete
};

class ChunkMesh
{
private:
	std::unique_ptr<Mesh> m_Mesh;

	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
	unsigned int m_indexIndex = 0;

	std::shared_ptr<std::mutex> m_MutexLock;

public:
	ChunkMeshState m_ChunkMeshState = ChunkMeshState::Ungenerated;

	ChunkMesh(const std::shared_ptr<std::mutex>& mutexLock) noexcept;
	
	void Bind() const noexcept;
	void Unbind() const noexcept;

	void Generate(const Chunk* chunk, const World* world, const glm::ivec2& chunkPosition) noexcept;
	void AddBlockFace(const glm::vec3& position, const float* vertices, float face) noexcept;
	void BufferMesh() noexcept;

	void Render(const ViewFrustum& frustum, const glm::ivec2& chunkPosition, const glm::vec3& playerPosition) noexcept;
};