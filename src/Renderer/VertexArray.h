#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID, m_VertexAttribPos;

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& vertexBufferLayout);

	void Bind() const;
	void Unbind() const;

	void ResetVertexAttribPos();
};
