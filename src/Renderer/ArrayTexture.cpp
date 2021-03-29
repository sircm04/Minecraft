#include "../pch.h"
#include "ArrayTexture.h"

ArrayTexture::ArrayTexture(const std::vector<std::string>& filePaths, int width, int height)
	: m_RendererID(0), m_FilePaths(filePaths), m_LocalBuffer(nullptr), m_Width(width), m_Height(height)
{
	stbi_set_flip_vertically_on_load(1);

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	float maxAniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY,
		5,
		GL_RGBA8,
		m_Width, m_Height,
		(GLsizei) filePaths.size()
	);

	unsigned char* image;
	int w, h, BPP;
	for (int i = 0; i < filePaths.size(); ++i) {
		std::string filePath = filePaths[i];

		image = stbi_load(filePath.c_str(), &w, &h, &BPP, 4);

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,
			0, 0, i,
			w, h, 1,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image);
	}

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

ArrayTexture::~ArrayTexture()
{
	glDeleteTextures(1, &m_RendererID);
}

void ArrayTexture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
}

void ArrayTexture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
