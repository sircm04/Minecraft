#pragma once

#include "Assets.h"
#include "Mesh.h"

namespace TextRenderer
{
	static const std::string CHARACTERS = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTYVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	static const uint8_t MAX_LENGTH = 16, CHARACTER_WIDTH = 6, CHARACTER_HEIGHT = 7;

	static inline void RenderText(const std::string& text, const glm::vec2& position, unsigned int size)
	{
		Assets::TEXTURES["FONT"]->Bind();

		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		unsigned int index = 0;

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
				0.0f + i, 1.0f, xMin, yMax,
				1.0f + i, 0.0f, xMax, yMin,
				0.0f + i, 0.0f, xMin, yMin,
				1.0f + i, 1.0f, xMax, yMax
			});

			indices.insert(indices.end(), {
				index, index + 1, index + 2,
				index, index + 3, index + 1
			});
			index += 4;
		}

		VertexBufferLayout guiLayout;
		guiLayout.Push<float>(2);
		guiLayout.Push<float>(2);

		auto mesh = std::make_unique<Mesh>(vertices, indices, guiLayout);

		Assets::SHADERS["GUI"]->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
		{
			position.x,
			position.y,
			0.0f
		}), { CHARACTER_WIDTH * size, CHARACTER_HEIGHT * size, 0.0f }));

		mesh->Bind();
		mesh->Render();
	}
}