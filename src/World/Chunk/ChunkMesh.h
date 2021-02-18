#pragma once

#include "../../Renderer/Mesh.h"
#include "../Block/Block.h"

#include "../../Math/Frustum.h"

class World;
class Chunk;

inline constexpr std::array<float, 36> FRONT_BLOCK_FACE_VERTICES = {
	// front
	0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f
};

inline constexpr std::array<float, 36> BACK_BLOCK_FACE_VERTICES {
	// back
	1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
};

inline constexpr std::array<float, 36> LEFT_BLOCK_FACE_VERTICES {
	// left
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f
};

inline constexpr std::array<float, 36> RIGHT_BLOCK_FACE_VERTICES {
	// right
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f
};

inline constexpr std::array<float, 36> TOP_BLOCK_FACE_VERTICES {
	// top
	1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
};

inline constexpr std::array<float, 36> BOTTOM_BLOCK_FACE_VERTICES {
	// bottom
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f
};

enum class ChunkMeshState : uint8_t
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
	unsigned int m_IndicesIndex = 0;

	std::shared_ptr<std::mutex> m_MutexLock;

public:
	ChunkMeshState m_ChunkMeshState = ChunkMeshState::Ungenerated;

	ChunkMesh(const std::shared_ptr<std::mutex>& mutexLock) noexcept;
	
	void Bind() const noexcept;
	void Unbind() const noexcept;

	void Generate(const Chunk* chunk, const World* world, const glm::ivec2& chunkPosition) noexcept;
	inline void AddBlockFace(const glm::vec3& position, const std::array<float, 36>& vertices, float face) noexcept;
	void BufferMesh() noexcept;

	void Render(const ViewFrustum& frustum, const glm::ivec2& chunkPosition, const glm::vec3& playerPosition) noexcept;
};