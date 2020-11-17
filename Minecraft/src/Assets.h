#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "ArrayTexture.h"
#include "VertexBufferLayout.h"

#include "Utils.h"

namespace Assets
{
	inline std::unique_ptr<Shader> SHADER, ENTITY_SHADER, GUI_SHADER, SUN_SHADER;

	inline std::unique_ptr<ArrayTexture> BLOCK_ARRAY_TEXTURE;

	inline std::unique_ptr<Mesh> GUI_MESH;
	inline std::unique_ptr<Texture> HEART_TEXTURE;
	inline std::unique_ptr<Texture> DIRT_TEXTURE;
	inline std::unique_ptr<Texture> CROSSHAIR_TEXTURE;

	inline std::unique_ptr<Mesh> SUN_MESH;
	inline std::unique_ptr<ArrayTexture> SUN_TEXTURE;

	inline std::unique_ptr<Mesh> COW_MESH;
	inline std::unique_ptr<Texture> COW_TEXTURE;

	inline void InitializeAssets()
	{
		SHADER = std::make_unique<Shader>(std::vector<ShaderEntry> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Lighting.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Lighting.frag") }
		});

		ENTITY_SHADER = std::make_unique<Shader>(std::vector<ShaderEntry> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Basic2dTex.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Basic2dTex.frag") }
		});

		GUI_SHADER = std::make_unique<Shader>(std::vector<ShaderEntry> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/GUI.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/GUI.frag") }
		});

		SUN_SHADER = std::make_unique<Shader>(std::vector<ShaderEntry> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Basic3dTex.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Basic3dTex.frag") }
		});

		GUI_SHADER->SetVec4("u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		std::vector<std::string> paths = {
			"grass_side.png", "grass_top.png", "grass_bottom.png", "stone.png",
			"cobblestone.png", "bedrock.png", "wood.png", "log_side.png", "log_top.png"
		};

		for (int i = 0; i < paths.size(); i++)
			paths[i] = ("res/images/" + paths[i]);

		BLOCK_ARRAY_TEXTURE = std::make_unique<ArrayTexture>(paths, 16, 16);

		VertexBufferLayout guiLayout;
		guiLayout.Push<float>(2);
		guiLayout.Push<float>(2);

		GUI_MESH = std::make_unique<Mesh>(std::vector<float> {
			0.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 0.0f
		}, std::vector<unsigned int> { 0, 1, 2, 0, 3, 1 }, guiLayout);

		HEART_TEXTURE = std::make_unique<Texture>("res/images/heart.png");
		DIRT_TEXTURE = std::make_unique<Texture>("res/images/grass_bottom.png");
		CROSSHAIR_TEXTURE = std::make_unique<Texture>("res/images/crosshair.png");

		VertexBufferLayout sunLayout;
		sunLayout.Push<float>(3);
		sunLayout.Push<float>(3);

		SUN_MESH = std::make_unique<Mesh>(std::vector<float> {
			1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f
		}, std::vector<unsigned int> { 0, 1, 2, 3, 4, 5 }, sunLayout); // East is positive x

		SUN_TEXTURE = std::make_unique<ArrayTexture>(std::vector<std::string> { "res/images/sun.png" }, 16, 16);

		VertexBufferLayout entityLayout;
		entityLayout.Push<float>(3);
		entityLayout.Push<float>(2);
		entityLayout.Push<float>(3);

		COW_MESH = std::make_unique<Mesh>(std::vector<float> {
			0.0f, 1.0f, 1.0f, 0.09375f, 0.8125f, 0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, 1.0f, 0.09375f, 0.5625f, 0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 1.0f, 0.21875f, 0.5625f, 0.0f, 0.0f, -1.0f,
			1.0f, 1.0f, 1.0f, 0.21875f, 0.8125f, 0.0f, 0.0f, -1.0f
		}, std::vector<unsigned int> {
			0, 1, 2, 2, 3, 0
		}, entityLayout);

		COW_TEXTURE = std::make_unique<Texture>("res/images/cow.png");
	}
};