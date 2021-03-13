#include "../pch.h"
#include "InventoryMenu.h"

#include "../Assets.h"

void InventoryMenu::Render(int width, int height, int scale, glm::mat4 guiProjection) const
{
	glClear(GL_DEPTH_BUFFER_BIT);

	Assets::SHADERS["GUI_ARRAY"]->Bind();
	Assets::SHADERS["GUI_ARRAY"]->SetMat4("projection", guiProjection);
	Assets::MESHES["GUI"]->Bind();

	Assets::ARRAY_TEXTURES["ITEMS"]->Bind();

	Assets::SHADERS["GUI_ARRAY"]->SetVec4("u_color", { 1.0f, 1.0f, 1.0f, 1.0f });
	for (int i = 0; i < m_Inventory->items.size(); ++i)
	{
		auto item = m_Inventory->items[i];

		if (item)
		{
			Assets::SHADERS["GUI_ARRAY"]->SetMat4("model", glm::scale(glm::translate(glm::mat4(1.0f),
			{
				(width * 0.5f) + (i % 5) * (16.0f * scale),
				(height * 0.5f) + (i / 5) * (16.0f * scale),
				0.0f
			}),
			{
				(16.0f * scale),
				(16.0f * scale),
				0.0f
			}));
			Assets::SHADERS["GUI_ARRAY"]->SetFloat("texZ", static_cast<float>(item->m_ItemType));
			Assets::MESHES["GUI"]->Render();
		}
	}
}