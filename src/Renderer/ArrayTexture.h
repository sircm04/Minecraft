#pragma once

class ArrayTexture
{
private:
	unsigned int m_RendererID, m_Width, m_Height;

public:
	ArrayTexture(const std::vector<std::string>& filePaths, int width, int height);
	~ArrayTexture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
};
