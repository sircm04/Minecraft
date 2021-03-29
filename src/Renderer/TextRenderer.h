#pragma once

#include "../Assets.h"
#include "Mesh.h"

namespace TextRenderer
{
	struct Margins
	{
		uint8_t width, height;
	};

	static const std::string CHARACTERS = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	static std::unordered_map<char, Margins> MARGINS =
	{
		{ ' ', { 4, 7 } },
		{ '!', { 2, 7 } },
		{ '"', { 5, 7 } },
		{ '\'', { 3, 7 } },
		{ '(', { 5, 7 } },
		{ ')', { 5, 7 } },
		{ '*', { 5, 7 } },
		{ '+', { 6, 7 } },
		{ ',', { 2, 8 } },
		{ '.', { 2, 7 } },
		{ ':', { 2, 7 } },
		{ ';', { 2, 8 } },
		{ '<', { 5, 8 } },
		{ '>', { 5, 8 } },
		{ 'f', { 5, 8 } },
		{ 'g', { 6, 8 } },
		{ 'i', { 2, 7 } },
		{ 'j', { 6, 8 } },
		{ 'k', { 5, 8 } },
		{ 'l', { 3, 8 } },
		{ 'p', { 6, 8 } },
		{ 'q', { 6, 8 } },
		{ 't', { 4, 8 } },
		{ 'y', { 6, 8 } },
		{ '@', { 7, 7 } },
		{ 'I', { 4, 7 } },
		{ '[', { 4, 7 } },
		{ ']', { 4, 7 } },
		{ '_', { 6, 8 } },
		{ '`', { 3, 7 } },
		{ '|', { 3, 7 } },
		{ '}', { 5, 7 } }
	};
	static constexpr Margins DEFAULT_MARGINS = { 6, 7 };
	static const uint8_t MAX_LENGTH = 16;
	static std::vector<float> vertices;
	static std::vector<unsigned int> indices;
	static unsigned int index = 0;

	static void Begin()
	{
		vertices.clear();
		indices.clear();
		index = 0;
	}

	static void DrawText(const std::string& text, const glm::vec2& position, unsigned int size)
	{
		unsigned int xPos = 0;

		for (int i = 0; i < text.length(); ++i)
		{
			const int charPosition = CHARACTERS.find(text[i]),
				x = (charPosition % MAX_LENGTH) * 8,
				y = (charPosition / MAX_LENGTH) * 8;

			static float imageWidth = Assets::TEXTURES.at("FONT").GetWidth(),
				imageHeight = Assets::TEXTURES.at("FONT").GetHeight();

			Margins margins;
			if (MARGINS.find(text[i]) != MARGINS.end())
				margins = MARGINS[text[i]];
			else
				margins = DEFAULT_MARGINS;

			float xMin = ((float) x / (float) imageWidth),
				xMax = ((float) (x + margins.width) / (float) imageWidth),
				yMin = 1.0f - ((float) y / (float) imageHeight),
				yMax = 1.0f - ((float) (y + margins.height) / (float) imageHeight);

			unsigned int xScale = (margins.width * size),
				yScale = (margins.height * size);

			vertices.insert(vertices.end(), {
				position.x + xPos, position.y + yScale, xMin, yMax,
				position.x + xPos + xScale, position.y, xMax, yMin,
				position.x + xPos, position.y, xMin, yMin,
				position.x + xPos + xScale, position.y + yScale, xMax, yMax
			});

			indices.insert(indices.end(), {
				index, index + 1, index + 2,
				index, index + 3, index + 1
			});
			index += 4;

			xPos += xScale;
		}
	}

	static void End()
	{
		VertexBufferLayout guiLayout;
		guiLayout.Push<float>(2);
		guiLayout.Push<float>(2);

		auto mesh = Mesh(vertices, indices, guiLayout);

		Assets::TEXTURES.at("FONT").Bind();
		Assets::SHADERS.at("GUI").SetMat4("model", glm::mat4(1.0f));

		mesh.Bind();
		mesh.Render();
	}
}
