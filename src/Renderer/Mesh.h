#pragma once

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"

class Mesh
{
public:
	Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const VertexBufferLayout& layout);
	Mesh(unsigned int vertexSize, unsigned int indexCount, const VertexBufferLayout& layout);
	
	void Bind() const;
	void Unbind() const;

	void Render(int mode = GL_TRIANGLES) const;

private:
	VertexArray m_VertexArray;
	VertexBuffer m_VertexBuffer;
	IndexBuffer m_IndexBuffer;
};
