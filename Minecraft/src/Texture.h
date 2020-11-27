#pragma once

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;

public:
	Texture(const std::string& filePath);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const noexcept { return m_Width; }
	inline int GetHeight() const noexcept { return m_Height; }
	inline int GetBPP() const noexcept { return m_BPP; }
};