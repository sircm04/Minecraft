#pragma once

class ArrayTexture
{
private:
	unsigned int m_RendererID;
	std::vector<std::string> m_FilePaths;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height;

public:
	ArrayTexture(const std::vector<std::string>& filePaths, int width, int height);
	~ArrayTexture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	int GetWidth() const noexcept { return m_Width; }
	int GetHeight() const noexcept { return m_Height; }
};
