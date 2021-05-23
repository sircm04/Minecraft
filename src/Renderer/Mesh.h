#pragma once

#include "../pch.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Mesh.h"

template <typename T>
struct MeshData
{
	std::vector<T> vertices;
	std::vector<unsigned int> indices;
};

template <typename T>
class Mesh
{
public:
	Mesh()
		: m_Index(0)
	{
	}

	void AddData(const MeshData<T>& data)
	{
		std::lock_guard lock(m_Mutex);
		m_Data.vertices.insert(m_Data.vertices.end(), data.vertices.begin(), data.vertices.end());
		std::vector<unsigned int> indices = data.indices;
		for (int i = 0; i < indices.size(); ++i)
			indices[i] = indices[i] + m_Index;
		m_Data.indices.insert(m_Data.indices.end(), indices.begin(), indices.end());
		m_Index += data.vertices.size();
	}

	void Clear()
	{
		m_Data.vertices.clear();
		m_Data.indices.clear();
		m_Index = 0;
	}

	void Buffer(const VertexBufferLayout& layout)
	{
		std::lock_guard lock(m_Mutex);
		m_VertexArray = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(m_Data.vertices);
		m_VertexArray->AddBuffer(*m_VertexBuffer, layout);
		m_IndexBuffer = std::make_unique<IndexBuffer>(m_Data.indices);
		Clear();
	}

	void Render(int mode = GL_TRIANGLES) const
	{
		m_VertexArray->Bind();
		glDrawElements(mode, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

private:
	MeshData<T> m_Data;
	std::unique_ptr<VertexArray> m_VertexArray;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	unsigned int m_Index;

	std::mutex m_Mutex;
};
