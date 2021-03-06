#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID, m_Count;

public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	unsigned int GetCount() const noexcept { return m_Count; }
};
