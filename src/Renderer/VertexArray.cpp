#include "../pch.h"
#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_RendererID);
	glBindVertexArray(m_RendererID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::AddBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& vertexBufferLayout)
{
	Bind();
	vertexBuffer.Bind();
	const auto& elements = vertexBufferLayout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); ++i)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(m_VertexAttribPos + i);
		glVertexAttribPointer(m_VertexAttribPos + i, element.count, element.type,
			element.normalized, vertexBufferLayout.GetStride(), (const void*) offset);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	m_VertexAttribPos += elements.size();
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::ResetVertexAttribPos()
{
	m_VertexAttribPos = 0;
}
