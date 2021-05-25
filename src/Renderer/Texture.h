#pragma once

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	int m_Width, m_Height, m_BPP;

public:
	Texture(const std::string& filePath);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int GetBPP() const { return m_BPP; }
};
