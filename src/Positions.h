#pragma once

typedef glm::vec3 WorldPosition;
typedef glm::ivec3 ChunkPosition;
typedef glm::ivec2 ChunkLocation;

namespace PosUtils
{
	static ChunkLocation ConvertWorldPosToChunkLoc(WorldPosition position)
	{
		return {
			(static_cast<int>(position.x) >> 4),
			(static_cast<int>(position.z) >> 4)
		};
	}

	static ChunkPosition ConvertWorldPosToChunkPos(WorldPosition position)
	{
		return {
			(static_cast<int>(position.x) & 15),
			position.y,
			(static_cast<int>(position.z) & 15)
		};
	}

	static ChunkLocation ConvertWorldPosToChunkLoc(uint16_t x, uint16_t z)
	{
		return {
			(static_cast<int>(x) >> 4),
			(static_cast<int>(z) >> 4)
		};
	}
}
