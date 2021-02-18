#include "../pch.h"
#include "Mesh.h"

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const VertexBufferLayout& layout)
	: m_VertexBuffer(&vertices[0], vertices.size() * sizeof(float)), m_IndexBuffer(&indices[0], indices.size())
{
	m_VertexArray.AddBuffer(m_VertexBuffer, layout);
}

Mesh::Mesh(unsigned int vertexSize, unsigned int indexCount, const VertexBufferLayout& layout)
	: m_VertexBuffer(nullptr, vertexSize), m_IndexBuffer(nullptr, indexCount)
{
	m_VertexArray.AddBuffer(m_VertexBuffer, layout);
}

void Mesh::Bind()
{
	m_VertexArray.Bind();
	m_IndexBuffer.Bind();
}

void Mesh::Unbind()
{
	m_VertexArray.Unbind();
	m_IndexBuffer.Unbind();
}

void Mesh::Render()
{
	glDrawElements(GL_TRIANGLES, m_IndexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr);
}