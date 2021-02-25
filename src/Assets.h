#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Renderer/ArrayTexture.h"
#include "Renderer/VertexBufferLayout.h"
#include "Utils/Utils.h"

namespace Assets
{
	inline std::unordered_map<std::string, std::unique_ptr<Shader>> SHADERS;
	inline std::unordered_map<std::string, std::unique_ptr<Texture>> TEXTURES;
	inline std::unordered_map<std::string, std::unique_ptr<ArrayTexture>> ARRAY_TEXTURES;
	inline std::unordered_map<std::string, std::unique_ptr<Mesh>> MESHES;

	static void InitializeAssets()
	{
		SHADERS["GENERIC"] = std::make_unique<Shader>(std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Lighting.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Lighting.frag") }
		});

		SHADERS["ENTITY"] = std::make_unique<Shader>(std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Basic2dTex.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Basic2dTex.frag") }
		});

		SHADERS["GUI"] = std::make_unique<Shader>(std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/GUI.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/GUI.frag") }
		});

		SHADERS["SUN"] = std::make_unique<Shader>(std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Basic3dTex.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Basic3dTex.frag") }
		});

		SHADERS["GUI"]->SetVec4("u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		ARRAY_TEXTURES["BLOCKS"] = std::make_unique<ArrayTexture>(std::vector<std::string> {
			"res/images/grass_side.png", "res/images/grass_top.png", "res/images/grass_bottom.png",
			"res/images/stone.png", "res/images/cobblestone.png", "res/images/bedrock.png",
			"res/images/wood.png", "res/images/log_side.png", "res/images/log_top.png"
		}, 16, 16);

		VertexBufferLayout guiLayout;
		guiLayout.Push<float>(2);
		guiLayout.Push<float>(2);

		MESHES["GUI"] = std::make_unique<Mesh>(std::vector<float> {
			0.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 0.0f
		}, std::vector<unsigned int> {
			0, 1, 2, 0, 3, 1
		}, guiLayout);

		TEXTURES["FONT"] = std::make_unique<Texture>("res/images/font.png");
		TEXTURES["HEART"] = std::make_unique<Texture>("res/images/heart.png");
		TEXTURES["DIRT"] = std::make_unique<Texture>("res/images/grass_bottom.png");
		TEXTURES["CROSSHAIR"] = std::make_unique<Texture>("res/images/crosshair.png");

		VertexBufferLayout sunLayout;
		sunLayout.Push<float>(3);
		sunLayout.Push<float>(3);

		MESHES["SUN"] = std::make_unique<Mesh>(std::vector<float> {
			0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f
		}, std::vector<unsigned int> {
			0, 1, 2, 3, 2, 1
		}, sunLayout); // East is positive x

		ARRAY_TEXTURES["SUN"] = std::make_unique<ArrayTexture>(std::vector<std::string> { "res/images/sun.png" }, 16, 16);

		VertexBufferLayout entityLayout;
		entityLayout.Push<float>(3);
		entityLayout.Push<float>(2);
		entityLayout.Push<float>(3);

		MESHES["COW"] = std::make_unique<Mesh>(std::vector<float> {
			-0.25f, 0.25f, 0.25f, 0.09375f, 0.8125f, 0.0f, 0.0f, -1.0f,
			-0.25f, -0.25f, 0.25f, 0.09375f, 0.5625f, 0.0f, 0.0f, -1.0f,
			0.25f, -0.25f, 0.25f, 0.21875f, 0.5625f, 0.0f, 0.0f, -1.0f,
			0.25f, 0.25f, 0.25f, 0.21875f, 0.8125f, 0.0f, 0.0f, -1.0f
		}, std::vector<unsigned int> {
			0, 1, 2, 2, 3, 0
		}, entityLayout);

		TEXTURES["COW"] = std::make_unique<Texture>("res/images/cow.png");
	}
};