#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "VertexBufferLayout.h"

namespace Assets
{
	inline std::unique_ptr<Mesh> COW_MESH;
	inline std::unique_ptr<Texture> COW_TEXTURE;

	inline void InitializeAssets()
	{
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		layout.Push<float>(3);

		COW_MESH = std::make_unique<Mesh>(std::vector<float> {
			0.0f, 1.0f, 1.0f, 0.09375f, 0.8125f, 0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, 1.0f, 0.09375f, 0.5625f, 0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 1.0f, 0.21875f, 0.5625f, 0.0f, 0.0f, -1.0f,
			1.0f, 1.0f, 1.0f, 0.21875f, 0.8125f, 0.0f, 0.0f, -1.0f
		}, std::vector<unsigned int> {
			0, 1, 2, 2, 3, 0
		}, layout);

		COW_TEXTURE = std::make_unique<Texture>("res/images/cow.png");
	}
};