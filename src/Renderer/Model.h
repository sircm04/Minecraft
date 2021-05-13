#pragma once

#include "../pch.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Mesh.h"

template<typename T>
class Model
{
public:
	Model()
		: m_Index(0)
	{
	}

	void AddMesh(const Mesh<T>& mesh)
	{
		std::lock_guard lock(m_Mutex);
		m_Mesh.vertices.insert(m_Mesh.vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
		std::vector<unsigned int> indices = mesh.indices;
		for (int i = 0; i < indices.size(); ++i)
			indices[i] = indices[i] + m_Index;
		m_Mesh.indices.insert(m_Mesh.indices.end(), indices.begin(), indices.end());
		m_Index += mesh.vertices.size();
	}

	void Buffer(const VertexBufferLayout& layout)
	{
		std::lock_guard lock(m_Mutex);
		m_VertexArray = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(m_Mesh.vertices);
		m_VertexArray->AddBuffer(*m_VertexBuffer, layout);
		m_IndexBuffer = std::make_unique<IndexBuffer>(m_Mesh.indices);
		m_Mesh.vertices.clear();
		m_Mesh.indices.clear();
	}

	void Render(int mode = GL_TRIANGLES) const
	{
		m_VertexArray->Bind();
		glDrawElements(mode, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

private:
	Mesh<T> m_Mesh;
	std::unique_ptr<VertexArray> m_VertexArray;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	unsigned int m_Index;

	std::mutex m_Mutex;
};
