#pragma once

#include "../../Renderer/Mesh.h"
#include "../Block/Block.h"
#include "../../Math/Frustum.h"

class World;
class Chunk;

inline constexpr std::array<float, 60> FRONT_BLOCK_FACE_VERTICES = {
	// front
	0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
};

inline constexpr std::array<float, 60> BACK_BLOCK_FACE_VERTICES {
	// back
	1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
};

inline constexpr std::array<float, 60> LEFT_BLOCK_FACE_VERTICES {
	// left
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

inline constexpr std::array<float, 60> RIGHT_BLOCK_FACE_VERTICES {
	// right
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

inline constexpr std::array<float, 60> TOP_BLOCK_FACE_VERTICES {
	// top
	1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
};

inline constexpr std::array<float, 60> BOTTOM_BLOCK_FACE_VERTICES {
	// bottom
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
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

	constexpr int CalculateVertexAO(int side1, int side2, int corner)
	{
		if (side1 && side2)
			return 0;

		return 3 - (side1 + side2 + corner);
	}

	bool DoesBlockExist(const World* world, glm::vec3 position);

public:
	static const VertexBufferLayout* getLayoutInstance() {
		static VertexBufferLayout* layoutInstance;
		if (!layoutInstance)
		{
			layoutInstance = new VertexBufferLayout();
			layoutInstance->Push<float>(3);
			layoutInstance->Push<float>(3);
			layoutInstance->Push<float>(3);
			layoutInstance->Push<float>(1);
		}
		return layoutInstance;
	}

	ChunkMeshState m_ChunkMeshState = ChunkMeshState::Ungenerated;

	ChunkMesh(const std::shared_ptr<std::mutex>& mutexLock) noexcept;
	
	void Bind() const noexcept;
	void Unbind() const noexcept;

	void Generate(const Chunk* chunk, const World* world, const glm::ivec2& chunkPosition) noexcept;
	void AddBlockFace(const World* world, const glm::vec3& position, const std::array<float, 60>& vertices, float face) noexcept;
	void BufferMesh() noexcept;
	void ClearMesh() noexcept;

	void Render(const ViewFrustum& frustum, const glm::ivec2& chunkPosition) noexcept;
};