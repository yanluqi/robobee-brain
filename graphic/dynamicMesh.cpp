#include "include/dynamicMesh.h"

DynamicMesh::DynamicMesh(std::vector<glm::vec3> vertex_data, std::vector<glm::vec2> uv_data)
{
	m_drawCount = vertex_data.size();

	glGenVertexArrays(1, &m_vertexArrayObject);
	
	glBindVertexArray(m_vertexArrayObject);

	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);

	SendData(vertex_data, uv_data);

    glBindVertexArray(0);
}

void DynamicMesh::Draw(std::vector<glm::vec3> vertex_data, std::vector<glm::vec2> uv_data)
{
	glBindVertexArray(m_vertexArrayObject);

	SendData(vertex_data, uv_data);

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, m_drawCount);

	// glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

    glBindVertexArray(0);
}

void DynamicMesh::SendData(std::vector<glm::vec3> vertex_data, std::vector<glm::vec2> uv_data)
{
	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(glm::vec3), &vertex_data[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // UV
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[UV]);
    glBufferData(GL_ARRAY_BUFFER, uv_data.size() * sizeof(glm::vec2), &uv_data[0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}