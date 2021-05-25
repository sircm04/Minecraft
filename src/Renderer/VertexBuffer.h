#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;

public:
	template <typename T>
	VertexBuffer(const std::vector<T>& data)
		: m_RendererID(0)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
	}

	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};
