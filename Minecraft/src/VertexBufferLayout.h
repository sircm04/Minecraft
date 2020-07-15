#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

struct VertexBufferElement
{
	unsigned int type, count;
	unsigned char normalized;

	VertexBufferElement(unsigned int type, unsigned int count, unsigned char normalized)
		: type(type), count(count), normalized(normalized)
	{
	}

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
			case GL_FLOAT:			return sizeof(GLfloat);
			case GL_UNSIGNED_INT:	return sizeof(GLuint);
			case GL_UNSIGNED_BYTE:	return sizeof(GLbyte);
		}
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;

public:
	VertexBufferLayout()
		: m_Stride(0)
	{
	}

	template<typename T>
	void Push(unsigned int count)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_Elements.emplace_back(GL_FLOAT, count, GL_FALSE);
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.emplace_back(GL_UNSIGNED_INT, count, GL_FALSE);
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.emplace_back(GL_UNSIGNED_BYTE, count, GL_TRUE);
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement> GetElements() const&
	{
		return m_Elements;
	}

	inline unsigned int GetStride() const&
	{
		return m_Stride;
	}
};