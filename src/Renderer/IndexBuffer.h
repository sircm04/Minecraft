#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID, m_Count;

public:
	IndexBuffer(const std::vector<unsigned int>& data);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	unsigned int GetCount() const { return m_Count; }
};
