#include "../../pch.h"
#include "Chunk.h"

#include "../World.h"
#include "../../Math/Frustum.h"
#include "../../Physics/AABB.h"

static constexpr inline AABB CHUNK_AABB = AABB({ 0, 0, 0 }, { Chunk::CHUNK_WIDTH, Chunk::CHUNK_HEIGHT, Chunk::CHUNK_DEPTH });

Chunk::Chunk()
	: m_ChunkState(ChunkState::Ungenerated), m_Model(std::make_unique<Model<Vertex>>())
{
}

void Chunk::Generate(const siv::PerlinNoise& noise, const ChunkLocation& location)
{
	m_ChunkState = ChunkState::Ungenerated;

	m_Blocks.reserve(Chunk::CHUNK_WIDTH * Chunk::CHUNK_HEIGHT * Chunk::CHUNK_DEPTH);

	const ChunkLocation realLocation = (location << 4);

	for (uint8_t x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		for (uint8_t z = 0; z < Chunk::CHUNK_DEPTH; ++z)
		{
			const double random = noise.noise0_1(((realLocation.x + x) * 0.025), ((realLocation.y + z) * 0.025)) * 50;
			const uint8_t grassHeight = (static_cast<uint8_t>(Chunk::CHUNK_HEIGHT * 0.33f) >> 2) + random,
				dirtHeight = (grassHeight - 3);

			for (uint8_t y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
			{
				if (y > grassHeight)
					SetBlock({ x, y, z }, { BlockType::Air });
				else if (y == grassHeight)
					SetBlock({ x, y, z }, { BlockType::Grass });
				else if (y >= dirtHeight)
					SetBlock({ x, y, z }, { BlockType::Dirt });
				else if (y > 0)
					SetBlock({ x, y, z }, { BlockType::Stone });
				else
					SetBlock({ x, y, z }, { BlockType::Bedrock });
			}
		}
	}

	m_ChunkState = ChunkState::Generated;
}

void Chunk::GenerateMesh(World& world, const ChunkLocation& location)
{
	m_ChunkState = ChunkState::GeneratingMesh;

	const ChunkLocation realChunkLocation = location << 4;

	const Chunk* frontChunk = world.GetChunk({ location.x, location.y + 1 });
	const Chunk* rightChunk = world.GetChunk({ location.x + 1, location.y });

	for (uint8_t x = 0; x < Chunk::CHUNK_WIDTH; ++x)
	{
		bool xNotFinished = (x != (Chunk::CHUNK_WIDTH - static_cast<uint8_t>(1)));

		for (uint8_t y = 0; y < Chunk::CHUNK_HEIGHT; ++y)
		{
			for (uint8_t z = 0; z < Chunk::CHUNK_DEPTH; ++z)
			{
				const Block* block = GetBlock({ x, y, z });
				const auto& blockData = block->GetBlockTypeData();

				const glm::ivec3 frontBlockPosition = { x, y, z + 1 };
				const glm::ivec3 rightBlockPosition = { x + 1, y, z };
				const glm::ivec3 topBlockPosition = { x, y + 1, z };
				const Block* frontBlock = ((z != Chunk::CHUNK_DEPTH - static_cast<uint8_t>(1)) ? GetBlock(frontBlockPosition)
					: frontChunk->GetBlock({ frontBlockPosition.x, frontBlockPosition.y, 0 }));
				const Block* rightBlock = (xNotFinished ? GetBlock(rightBlockPosition)
					: rightChunk->GetBlock({ 0, rightBlockPosition.y, rightBlockPosition.z }));
				const Block* topBlock = GetBlock(topBlockPosition);

				if (blockData.isSolid)
				{
					const glm::ivec3& realPosition = glm::ivec3 { x + realChunkLocation.x, y, z + realChunkLocation.y };

					if (frontBlock->GetBlockTypeData().isTransparent)
						AddFaceToMesh(world, FRONT_BLOCK_FACE, realPosition, blockData.faces.value()[0], !blockData.isTransparent);

 					if (rightBlock->GetBlockTypeData().isTransparent)
						AddFaceToMesh(world, RIGHT_BLOCK_FACE, realPosition, blockData.faces.value()[2], !blockData.isTransparent);

					if (!topBlock || topBlock->GetBlockTypeData().isTransparent)
						AddFaceToMesh(world, TOP_BLOCK_FACE, realPosition, blockData.faces.value()[4], !blockData.isTransparent);
				}

				if (blockData.isTransparent)
				{
					if (frontBlock->GetBlockTypeData().isSolid)
					{
						const auto& frontBlockData = frontBlock->GetBlockTypeData();
						AddFaceToMesh(world, BACK_BLOCK_FACE, { frontBlockPosition.x + realChunkLocation.x, frontBlockPosition.y, frontBlockPosition.z + realChunkLocation.y },
							frontBlockData.faces.value()[1], !frontBlockData.isTransparent);
					}

					if (rightBlock->GetBlockTypeData().isSolid)
					{
						const auto& rightBlockData = rightBlock->GetBlockTypeData();
						AddFaceToMesh(world, LEFT_BLOCK_FACE, { rightBlockPosition.x + realChunkLocation.x, rightBlockPosition.y, rightBlockPosition.z + realChunkLocation.y },
							rightBlockData.faces.value()[3], !rightBlockData.isTransparent);
					}

					if (topBlock && topBlock->GetBlockTypeData().isSolid)
					{
						const auto& topBlockData = topBlock->GetBlockTypeData();
						AddFaceToMesh(world, BOTTOM_BLOCK_FACE, { topBlockPosition.x + realChunkLocation.x, topBlockPosition.y, topBlockPosition.z + realChunkLocation.y },
							topBlockData.faces.value()[5], !topBlockData.isTransparent);
					}
				}
			}
		}
	}

	m_ChunkState = ChunkState::GeneratedMesh;
}

void Chunk::AddFaceToMesh(World& world, const BlockFace& face, const WorldPosition& position, float texture, bool doAO)
{
	Mesh<Vertex> mesh;

	for (uint8_t i = 0; i < 4; ++i)
	{
		auto& vertex = face.vertices[i];

		unsigned int ao = 3;

		if (doAO)
		{
			const Block* side1 = world.GetBlock(position + vertex.normal + vertex.perpendicularNormal1);
			const Block* side2 = world.GetBlock(position + vertex.normal + vertex.perpendicularNormal2);
			const Block* corner = world.GetBlock(position + vertex.normal + vertex.perpendicularNormal1 + vertex.perpendicularNormal2);

			bool side1Bool = side1 && side1->GetBlockTypeData().isSolid && !side1->GetBlockTypeData().isTransparent;
			bool side2Bool = side2 && side2->GetBlockTypeData().isSolid && !side2->GetBlockTypeData().isTransparent;
			bool cornerBool = corner && corner->GetBlockTypeData().isSolid && !corner->GetBlockTypeData().isTransparent;

			ao = CalculateVertexAO(side1Bool, side2Bool, cornerBool);
		}
		
		mesh.vertices.emplace_back(Vertex
		{
			vertex.position + position,
			{
				vertex.texcoords.x,
				vertex.texcoords.y,
				texture
			},
			vertex.normal,
			ao
		});
	}
	
	mesh.indices = {
		0, 1, 2, 2, 3, 0
	};

	m_Model->AddMesh(mesh);
}

void Chunk::BufferMesh()
{
	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(3);
	layout.Push<float>(3);
	layout.Push<unsigned int>(1);

	m_Model->Buffer(layout);

	m_ChunkState = ChunkState::Buffered;
}

void Chunk::Render(const ViewFrustum& frustum, const ChunkLocation& location)
{
	const ChunkLocation realLocation = location << 4;

	if (frustum.IsAABBInFrustum(CHUNK_AABB, { realLocation.x, 0, realLocation.y }))
		m_Model->Render();
}

void Chunk::SetBlock(const ChunkPosition& position, const Block& block)
{
	if (IsInBounds(position))
		m_Blocks[PositionToIndex(position)] = block;
}

Block* Chunk::GetBlock(const ChunkPosition& position)
{
	if (!IsInBounds(position))
		return nullptr;

	return &m_Blocks[PositionToIndex(position)];
}

const Block* Chunk::GetBlock(const ChunkPosition& position) const
{
	if (!IsInBounds(position))
		return nullptr;

	return &m_Blocks[PositionToIndex(position)];
}

Block* Chunk::GetHighestBlock(const ChunkPosition2D& position)
{
	for (uint8_t y = Chunk::CHUNK_HEIGHT - 1; y > 0; --y)
	{
		Block* block = GetBlock({ position.x, y, position.y });
		if (block->GetBlockTypeData().isSolid)
			return block;
	}

	return nullptr;
}

const Block* Chunk::GetHighestBlock(const ChunkPosition2D& position) const
{
	for (uint8_t y = Chunk::CHUNK_HEIGHT - 1; y > 0; --y)
	{
		const Block* block = GetBlock({ position.x, y, position.y });
		if (block->GetBlockTypeData().isSolid)
			return block;
	}

	return nullptr;
}

std::optional<uint8_t> Chunk::GetHighestBlockYPos(const ChunkPosition2D& position) const
{
	for (uint8_t y = Chunk::CHUNK_HEIGHT - 1; y > 0; --y)
		if (GetBlock({ position.x, y, position.y })->GetBlockTypeData().isSolid)
			return y;

	return std::nullopt;
}

uint16_t Chunk::PositionToIndex(const ChunkPosition& position) const
{
	return (static_cast<uint16_t>(position.y) << 8) | (static_cast<uint16_t>(position.z) << 4) | static_cast<uint16_t>(position.x);
}
