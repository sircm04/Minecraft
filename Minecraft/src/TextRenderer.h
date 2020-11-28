#pragma once

#include "Assets.h"
#include "Mesh.h"

namespace TextRenderer
{
	static const std::string CHARACTERS = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTYVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	static const uint8_t MAX_LENGTH = 16, CHARACTER_WIDTH = 8, CHARACTER_HEIGHT = 8;

	static inline void RenderText(const std::string& text, const glm::vec2& position, int size)
	{
		Assets::FONT_TEXTURE->Bind();

		VertexBufferLayout guiLayout;
		guiLayout.Push<float>(2);
		guiLayout.Push<float>(2);

		for (int i = 0; i < text.length(); i++)
		{
			const int charPosition = CHARACTERS.find(text[i]),
				x = (charPosition % MAX_LENGTH) * CHARACTER_WIDTH,
				y = (charPosition / MAX_LENGTH) * CHARACTER_HEIGHT;

			static int imageWidth = Assets::FONT_TEXTURE->GetWidth(),
				imageHeight = Assets::FONT_TEXTURE->GetHeight();

			float xMin = ((float) x / (float) imageWidth), xMax = ((float) (x + CHARACTER_WIDTH) / (float) imageWidth),
				yMin = 1.0f - ((float) y / (float) imageHeight), yMax = 1.0f - ((float) (y + CHARACTER_HEIGHT) / (float) imageHeight);

			auto mesh = std::make_unique<Mesh>(std::vector<float> {
				0.0f, 1.0f, xMin, yMax,
				1.0f, 0.0f, xMax, yMin,
				0.0f, 0.0f, xMin, yMin,
				1.0f, 1.0f, xMax, yMax
			}, std::vector<unsigned int> { 0, 1, 2, 0, 3, 1 }, guiLayout);

			Assets::GUI_SHADER->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
			{
				position.x + (i * size),
				position.y,
				0.0f
			}), { size, size, 0.0f }));

			mesh->Bind();
			mesh->Render();
		}
	}
}