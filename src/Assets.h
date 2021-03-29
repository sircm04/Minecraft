#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Renderer/ArrayTexture.h"
#include "Renderer/VertexBufferLayout.h"
#include "Utils/Utils.h"

namespace Assets
{
	inline std::unordered_map<std::string, const Shader> SHADERS;
	inline std::unordered_map<std::string, const Texture> TEXTURES;
	inline std::unordered_map<std::string, const ArrayTexture> ARRAY_TEXTURES;
	inline std::unordered_map<std::string, const Mesh> MESHES;

	static void InitializeAssets()
	{
		SHADERS.emplace("BLOCK", std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Block.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Block.frag") }
		});

		SHADERS.emplace("BLOCK_HITBOX", std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/BlockHitbox.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/BlockHitbox.frag") }
		});

		SHADERS.emplace("ENTITY", std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Basic2dTex.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Basic2dTex.frag") }
		});

		SHADERS.emplace("GUI", std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/GUI.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/GUI.frag") }
		});

		SHADERS.emplace("GUI_ARRAY", std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/GUIArray.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/GUIArray.frag") }
		});

		SHADERS.emplace("SUN", std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Sun.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Sun.frag") }
		});

		SHADERS.emplace("SKY", std::unordered_map<unsigned int, std::string> {
			{ GL_VERTEX_SHADER, Utils::ReadFile("res/shaders/Sky.vert") },
			{ GL_FRAGMENT_SHADER, Utils::ReadFile("res/shaders/Sky.frag") }
		});

		SHADERS.at("GUI").SetVec4("u_color", glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		ARRAY_TEXTURES.emplace(std::piecewise_construct, std::forward_as_tuple("BLOCKS"), std::forward_as_tuple(std::vector<std::string> {
			"res/images/grass_side.png", "res/images/grass_top.png", "res/images/grass_bottom.png",
			"res/images/stone.png", "res/images/cobblestone.png", "res/images/bedrock.png",
			"res/images/wood.png", "res/images/log_side.png", "res/images/log_top.png",
			"res/images/leaves.png"
		}, 16, 16));

		ARRAY_TEXTURES.emplace(std::piecewise_construct, std::forward_as_tuple("ITEMS"), std::forward_as_tuple(std::vector<std::string> {
			"res/images/bread.png", "res/images/diamond.png"
		}, 16, 16));

		VertexBufferLayout blockHitboxLayout;
		blockHitboxLayout.Push<float>(3);

		MESHES.emplace(std::piecewise_construct, std::forward_as_tuple("BLOCK_HITBOX"), std::forward_as_tuple(std::vector<float> {
			-0.001, -0.001, -0.001, // front bottom left
			1.001, -0.001, -0.001, // front bottom right
			1.001, 1.001, -0.001, // front top right
			-0.001, 1.001, -0.001, // front top left
			-0.001, -0.001, 1.0, // back bottom left
			1.001, -0.001, 1.001, // back bottom right
			1.001, 1.001, 1.001, // back top right
			-0.001, 1.001, 1.001  // back top left
		}, std::vector<unsigned int> {
			// front
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			// back
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			// sides
			0, 4,
			1, 5,
			2, 6,
			3, 7
		}, blockHitboxLayout));

		VertexBufferLayout guiLayout;
		guiLayout.Push<float>(2);
		guiLayout.Push<float>(2);

		MESHES.emplace(std::piecewise_construct, std::forward_as_tuple("GUI"), std::forward_as_tuple(std::vector<float> {
			0.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 0.0f
		}, std::vector<unsigned int> {
			0, 1, 2, 0, 3, 1
		}, guiLayout));

		TEXTURES.emplace("FONT", "res/images/font.png");
		TEXTURES.emplace("HEART", "res/images/heart.png");
		TEXTURES.emplace("DIRT", "res/images/grass_bottom.png");
		TEXTURES.emplace("CROSSHAIR", "res/images/crosshair.png");

		VertexBufferLayout sunLayout;
		sunLayout.Push<float>(3);
		sunLayout.Push<float>(3);

		MESHES.emplace(std::piecewise_construct, std::forward_as_tuple("SUN"), std::forward_as_tuple(std::vector<float> {
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f
		}, std::vector<unsigned int> {
			0, 1, 2, 3, 2, 1
		}, sunLayout)); // East is positive x

		TEXTURES.emplace("SUN", "res/images/sun.png");

		VertexBufferLayout skyLayout;
		skyLayout.Push<float>(3);

		MESHES.emplace(std::piecewise_construct, std::forward_as_tuple("SKY"), std::forward_as_tuple(std::vector<float> {
			// front
			0.5f, 0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			// back
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			// left
			-0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,
			// right
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			// top
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			// bottom
			0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f
		}, std::vector<unsigned int> {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4,
			8, 9, 10, 10, 11, 8,
			12, 13, 14, 14, 15, 12,
			16, 17, 18, 18, 19, 16,
			20, 21, 22, 22, 23, 20
		}, skyLayout));

		VertexBufferLayout entityLayout;
		entityLayout.Push<float>(3);
		entityLayout.Push<float>(2);
		entityLayout.Push<float>(3);

		MESHES.emplace(std::piecewise_construct, std::forward_as_tuple("COW"), std::forward_as_tuple(std::vector<float> {
			-0.25f, 0.25f, 0.25f, 0.09375f, 0.8125f, 0.0f, 0.0f, -1.0f,
			-0.25f, -0.25f, 0.25f, 0.09375f, 0.5625f, 0.0f, 0.0f, -1.0f,
			0.25f, -0.25f, 0.25f, 0.21875f, 0.5625f, 0.0f, 0.0f, -1.0f,
			0.25f, 0.25f, 0.25f, 0.21875f, 0.8125f, 0.0f, 0.0f, -1.0f
		}, std::vector<unsigned int> {
			0, 1, 2, 2, 3, 0
		}, entityLayout));

		TEXTURES.emplace("COW", "res/images/cow.png");
	}
};
