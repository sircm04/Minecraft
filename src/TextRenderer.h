#pragma once

#include "Assets.h"
#include "Mesh.h"

namespace TextRenderer
{
	static const std::string CHARACTERS = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTYVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	static const uint8_t MAX_LENGTH = 16, CHARACTER_WIDTH = 6, CHARACTER_HEIGHT = 7;
	static std::vector<float> vertices;
	static std::vector<unsigned int> indices;
	static unsigned int index = 0;

	static inline void Begin()
	{
		vertices.clear();
		indices.clear();
		index = 0;
	}

	static inline void DrawText(const std::string& text, const glm::vec2& position, unsigned int size)
	{
		unsigned int xScale = (CHARACTER_WIDTH * size),
			yScale = (CHARACTER_HEIGHT * size);

		for (int i = 0; i < text.length(); ++i)
		{
			const int charPosition = CHARACTERS.find(text[i]),
				x = (charPosition % MAX_LENGTH) * 8,
				y = (charPosition / MAX_LENGTH) * 8;

			static float imageWidth = Assets::TEXTURES["FONT"]->GetWidth(),
				imageHeight = Assets::TEXTURES["FONT"]->GetHeight();

			float xMin = ((float) x / (float) imageWidth),
				xMax = ((float) (x + CHARACTER_WIDTH) / (float) imageWidth),
				yMin = 1.0f - ((float) y / (float) imageHeight),
				yMax = 1.0f - ((float) (y + CHARACTER_HEIGHT) / (float) imageHeight);

			vertices.insert(vertices.end(), {
				i * xScale + position.x, 1.0f * yScale + position.y, xMin, yMax,
				(1.0f + i) * xScale + position.x, position.y, xMax, yMin,
				i * xScale + position.x, position.y, xMin, yMin,
				(1.0f + i) * xScale + position.x, 1.0f * yScale + position.y, xMax, yMax
			});

			indices.insert(indices.end(), {
				index, index + 1, index + 2,
				index, index + 3, index + 1
			});
			index += 4;
		}
	}

	static inline void End()
	{
		VertexBufferLayout guiLayout;
		guiLayout.Push<float>(2);
		guiLayout.Push<float>(2);

		auto mesh = Mesh(vertices, indices, guiLayout);

		Assets::TEXTURES["FONT"]->Bind();
		Assets::SHADERS["GUI"]->SetMat4("model", glm::mat4(1.0f));

		mesh.Bind();
		mesh.Render();
	}
}