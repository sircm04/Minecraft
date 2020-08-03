#include "pch.h"
#include "ChunkMesh.h"

#include "World.h"
#include "Chunk.h"

#include "Timer.h"

void ChunkMesh::Bind()
{
	m_Mesh->Bind();
}

void ChunkMesh::Unbind()
{
	m_Mesh->Unbind();
}

void ChunkMesh::Generate(Chunk* chunk, World* world, const glm::ivec2& chunkPosition)
{
	m_ChunkMeshState = ChunkMeshState::Ungenerated;

	m_Vertices.clear();
	m_Indices.clear();
	m_Vertices.shrink_to_fit();
	m_Indices.shrink_to_fit();
	m_indexIndex = 0;

	Chunk* frontChunk = world->GetChunk({ chunkPosition.x, chunkPosition.y + 1 });
	Chunk* rightChunk = world->GetChunk({ chunkPosition.x + 1, chunkPosition.y });

	glm::ivec2 realChunkPosition = { chunkPosition.x * Chunk::CHUNK_WIDTH, chunkPosition.y * Chunk::CHUNK_DEPTH };
	bool xNotFinished;

	for (int x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		xNotFinished = (x != Chunk::CHUNK_WIDTH_M1);

		for (int y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
		{
			for (int z = 0; z < Chunk::CHUNK_DEPTH; ++z)
			{
				glm::ivec3 position = glm::ivec3 { x, y, z };
				const Block* block = chunk->GetBlock(position);

				glm::ivec3 frontBlockPosition = { position.x, position.y, position.z + 1 };
				glm::ivec3 rightBlockPosition = { position.x + 1, position.y, position.z };
				glm::ivec3 topBlockPosition = { position.x, position.y + 1, position.z };
				const Block* frontBlock = ((z != Chunk::CHUNK_DEPTH_M1 || !frontChunk) ? chunk->GetBlock(frontBlockPosition) : frontChunk->GetBlock({
					frontBlockPosition.x, frontBlockPosition.y, 0 }));
				const Block* rightBlock = ((xNotFinished || !rightChunk) ? chunk->GetBlock(rightBlockPosition) : rightChunk->GetBlock({
					0, rightBlockPosition.y, rightBlockPosition.z }));
				const Block* topBlock = chunk->GetBlock(topBlockPosition);

				if (block->m_BlockType != BlockType::Air)
				{
					glm::ivec3 realPosition = glm::ivec3 { x + realChunkPosition.x, y, z + realChunkPosition.y };

					if (frontBlock && frontBlock->m_BlockType == BlockType::Air)
						AddBlockFace(realPosition, FRONT_BLOCK_FACE_VERTICES, block->GetBlockTypeData()->m_Faces.value()[0]);

					if (rightBlock && rightBlock->m_BlockType == BlockType::Air)
						AddBlockFace(realPosition, RIGHT_BLOCK_FACE_VERTICES, block->GetBlockTypeData()->m_Faces.value()[2]);

					if (!topBlock || topBlock->m_BlockType == BlockType::Air)
						AddBlockFace(realPosition, TOP_BLOCK_FACE_VERTICES, block->GetBlockTypeData()->m_Faces.value()[4]);
				}
				else
				{
					if (frontBlock && frontBlock->m_BlockType != BlockType::Air)
						AddBlockFace({ frontBlockPosition.x + realChunkPosition.x, frontBlockPosition.y, frontBlockPosition.z + realChunkPosition.y },
							BACK_BLOCK_FACE_VERTICES, frontBlock->GetBlockTypeData()->m_Faces.value()[1]);

					if (rightBlock && rightBlock->m_BlockType != BlockType::Air)
						AddBlockFace({ rightBlockPosition.x + realChunkPosition.x, rightBlockPosition.y, rightBlockPosition.z + realChunkPosition.y },
							LEFT_BLOCK_FACE_VERTICES, rightBlock->GetBlockTypeData()->m_Faces.value()[3]);

					if (topBlock && topBlock->m_BlockType != BlockType::Air)
						AddBlockFace({ topBlockPosition.x + realChunkPosition.x, topBlockPosition.y, topBlockPosition.z + realChunkPosition.y },
							BOTTOM_BLOCK_FACE_VERTICES, topBlock->GetBlockTypeData()->m_Faces.value()[5]);
				}
			}
		}
	}

	m_ChunkMeshState = ChunkMeshState::Generated;
}

void ChunkMesh::AddBlockFace(const glm::vec3& position, const std::vector<float>& vertices, float face)
{
	for (int i = 0, index = 0; i < 4; ++i) {
		m_Vertices.insert(m_Vertices.end(), {
			vertices[index++] + position.x,
			vertices[index++] + position.y,
			vertices[index++] + position.z,
			vertices[index++],
			vertices[index++],
			vertices[index++] + face,
			vertices[index++],
			vertices[index++],
			vertices[index++]
		});
	}

	m_Indices.insert(m_Indices.end(), {
		m_indexIndex, m_indexIndex + 1, m_indexIndex + 2,
		m_indexIndex + 2, m_indexIndex + 3, m_indexIndex
	});
	m_indexIndex += 4; // m_indexIndex += vertexCount;
}

void ChunkMesh::BufferMesh()
{
	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(3);
	layout.Push<float>(3);
	
	m_Mesh = std::make_unique<Mesh>(m_Vertices, m_Indices, layout);
	
	m_Vertices.clear();
	m_Indices.clear();
	m_Vertices.shrink_to_fit();
	m_Indices.shrink_to_fit();
	m_indexIndex = 0;

	m_ChunkMeshState = ChunkMeshState::Complete;
}

void ChunkMesh::Render()
{
	m_Mesh->Render();
}