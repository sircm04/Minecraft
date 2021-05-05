#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID;

	VertexBufferLayout m_VertexBufferLayout;

public:
	VertexArray(const VertexBufferLayout& vertexBufferLayout);
	~VertexArray();

	void AddBuffer(const VertexBuffer& vertexBuffer);

	void Bind() const;
	void Unbind() const;

	void ResetVertexAttribIndex();
};
