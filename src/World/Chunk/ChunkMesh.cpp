#include "../../pch.h"
#include "ChunkMesh.h"

#include "../World.h"
#include "Chunk.h"

void ChunkMesh::Bind() const noexcept
{
	m_Mesh->Bind();
}

void ChunkMesh::Unbind() const noexcept
{
	m_Mesh->Unbind();
}

void ChunkMesh::Generate(const Chunk* chunk, const World* world, const glm::ivec2& chunkPosition) noexcept
{
	m_ChunkMeshState = ChunkMeshState::Ungenerated;

	ClearMesh();

	const Chunk* frontChunk = world->GetChunk({ chunkPosition.x, chunkPosition.y + 1 });
	const Chunk* rightChunk = world->GetChunk({ chunkPosition.x + 1, chunkPosition.y });

	const glm::ivec2 realChunkPosition = { chunkPosition.x << 4, chunkPosition.y << 4 };

	for (uint8_t x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		bool xNotFinished = (x != (Chunk::CHUNK_WIDTH - static_cast<uint8_t>(1)));

		for (uint8_t y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
		{
			for (uint8_t z = 0; z < Chunk::CHUNK_DEPTH; ++z)
			{
				const Block* block = chunk->GetBlock({ x, y, z });
				const auto& blockData = block->GetBlockTypeData();

				const glm::ivec3 frontBlockPosition = { x, y, z + 1 };
				const glm::ivec3 rightBlockPosition = { x + 1, y, z };
				const glm::ivec3 topBlockPosition = { x, y + 1, z };
				const Block* frontBlock = ((z != (Chunk::CHUNK_DEPTH - static_cast<uint8_t>(1)) || !frontChunk) ? chunk->GetBlockInBounds(frontBlockPosition) : frontChunk->GetBlockInBounds({
					frontBlockPosition.x, frontBlockPosition.y, 0 }));
				const Block* rightBlock = ((xNotFinished || !rightChunk) ? chunk->GetBlockInBounds(rightBlockPosition) : rightChunk->GetBlockInBounds({
					0, rightBlockPosition.y, rightBlockPosition.z }));
				const Block* topBlock = chunk->GetBlockInBounds(topBlockPosition);

				if (blockData.isSolid)
				{
					const glm::ivec3& realPosition = glm::ivec3 { x + realChunkPosition.x, y, z + realChunkPosition.y };

					if (frontBlock && frontBlock->GetBlockTypeData().isTransparent)
						AddBlockFace(world, realPosition, FRONT_BLOCK_FACE_VERTICES, blockData.faces.value()[0], !blockData.isTransparent);

					if (rightBlock && rightBlock->GetBlockTypeData().isTransparent)
						AddBlockFace(world, realPosition, RIGHT_BLOCK_FACE_VERTICES, blockData.faces.value()[2], !blockData.isTransparent);

					if (!topBlock || topBlock->GetBlockTypeData().isTransparent)
						AddBlockFace(world, realPosition, TOP_BLOCK_FACE_VERTICES, blockData.faces.value()[4], !blockData.isTransparent);
				}
				
				if (blockData.isTransparent)
				{
					if (frontBlock && frontBlock->GetBlockTypeData().isSolid)
					{
						const auto& frontBlockData = frontBlock->GetBlockTypeData();
						AddBlockFace(world, { frontBlockPosition.x + realChunkPosition.x, frontBlockPosition.y, frontBlockPosition.z + realChunkPosition.y },
							BACK_BLOCK_FACE_VERTICES, frontBlockData.faces.value()[1], !frontBlockData.isTransparent);
					}

					if (rightBlock && rightBlock->GetBlockTypeData().isSolid)
					{
						const auto& rightBlockData = rightBlock->GetBlockTypeData();
						AddBlockFace(world, { rightBlockPosition.x + realChunkPosition.x, rightBlockPosition.y, rightBlockPosition.z + realChunkPosition.y },
							LEFT_BLOCK_FACE_VERTICES, rightBlockData.faces.value()[3], !rightBlockData.isTransparent);
					}

					if (topBlock && topBlock->GetBlockTypeData().isSolid)
					{
						const auto& topBlockData = topBlock->GetBlockTypeData();
						AddBlockFace(world, { topBlockPosition.x + realChunkPosition.x, topBlockPosition.y, topBlockPosition.z + realChunkPosition.y },
							BOTTOM_BLOCK_FACE_VERTICES, topBlockData.faces.value()[5], !topBlockData.isTransparent);
					}
				}
			}
		}
	}

	m_ChunkMeshState = ChunkMeshState::Generated;
}

bool ChunkMesh::DoesBlockExist(const World* world, glm::vec3 position) noexcept
{
	const Block* block = world->GetBlock(position);
	return ((block) ? block->GetBlockTypeData().isSolid : false);
}

void ChunkMesh::AddBlockFace(const World* world, const glm::vec3& position, const std::array<float, 60>& vertices, float face, bool doAO) noexcept
{
	static std::mutex m_Mutex;

	for (uint8_t i = 0, index = 0; i < 4; ++i)
	{
		glm::vec3 normal = {
			vertices[index + 6],
			vertices[index + 7],
			vertices[index + 8]
		};

		glm::vec3 perpendicularNormal1 = {
			vertices[index + 9],
			vertices[index + 10],
			vertices[index + 11]
		};

		glm::vec3 perpendicularNormal2 = {
			vertices[index + 12],
			vertices[index + 13],
			vertices[index + 14]
		};

		float ao = 3;

		if (doAO)
		{
			const Block* side1 = world->GetBlock(position + normal + perpendicularNormal1);
			const Block* side2 = world->GetBlock(position + normal + perpendicularNormal2);
			const Block* corner = world->GetBlock(position + normal + perpendicularNormal1 + perpendicularNormal2);

			bool side1Bool = side1 && side1->GetBlockTypeData().isSolid && !side1->GetBlockTypeData().isTransparent;
			bool side2Bool = side2 && side2->GetBlockTypeData().isSolid && !side2->GetBlockTypeData().isTransparent;
			bool cornerBool = corner && corner->GetBlockTypeData().isSolid && !corner->GetBlockTypeData().isTransparent;

			ao = static_cast<float>(CalculateVertexAO(side1Bool, side2Bool, cornerBool));
		}

		m_Vertices.insert(m_Vertices.end(),
		{
			vertices[index++] + position.x,
			vertices[index++] + position.y,
			vertices[index++] + position.z,
			vertices[index++],
			vertices[index++],
			vertices[index++] + face,
			normal.x,
			normal.y,
			normal.z,
			ao
		});

		index += 9;
	}

	m_Indices.insert(m_Indices.end(),
	{
		m_IndicesIndex, m_IndicesIndex + 1, m_IndicesIndex + 2,
		m_IndicesIndex + 2, m_IndicesIndex + 3, m_IndicesIndex
	});

	m_IndicesIndex += 4;
}

void ChunkMesh::BufferMesh() noexcept
{
	m_Mesh = std::make_unique<Mesh>(m_Vertices, m_Indices, *GetLayoutInstance());
	ClearMesh();

	m_ChunkMeshState = ChunkMeshState::Complete;
}

void ChunkMesh::ClearMesh() noexcept
{
	m_Vertices.clear();
	m_Indices.clear();
	m_IndicesIndex = 0;
}

void ChunkMesh::Render(const ViewFrustum& frustum, const glm::ivec2& chunkPosition) noexcept
{
	const glm::vec2 realChunkPosition = { (chunkPosition.x * Chunk::CHUNK_WIDTH), (chunkPosition.y * Chunk::CHUNK_DEPTH) };

	if (frustum.IsBoxInFrustum({ { 0, 0, 0 }, { Chunk::CHUNK_WIDTH, Chunk::CHUNK_HEIGHT, Chunk::CHUNK_DEPTH } },
		{
			realChunkPosition.x,
			0,
			realChunkPosition.y
		}))
		m_Mesh->Render();
}
