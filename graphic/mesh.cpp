#include "include/mesh.h"
#include <iostream>

Mesh::Mesh()
{
	
}

Mesh::Mesh(const std::string& filePath, Model& model)
{
	bool res = loadOBJ(filePath.c_str(), model.vertices, model.uvs, model.normals);

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;

	indexVBO(model.vertices, model.uvs, model.normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	model.SetVertices(indexed_vertices);
	model.SetUVs(indexed_uvs);
	model.SetNormals(indexed_normals);
	model.SetInidices(indices);

	m_drawCount = model.indices.size();
	hasUV = model.hasUVs;
	hasNorm = model.hasNormals;


	InitMesh(model);
}

Mesh::Mesh(Model& model)
{

	if(!model.indexed)
	{
		std::vector<unsigned short> indices;
		std::vector<glm::vec3> indexed_vertices;
		std::vector<glm::vec2> indexed_uvs;
		std::vector<glm::vec3> indexed_normals;
		indexVBO(model.vertices, model.uvs, model.normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
		model.SetVertices(indexed_vertices);
		model.SetUVs(indexed_uvs);
		model.SetNormals(indexed_normals);
		model.SetInidices(indices);

		std::cout << "Not Working" << std::endl;
	}

	m_drawCount = model.indices.size();
	hasUV = model.hasUVs;
	hasNorm = model.hasNormals;

	InitMesh(model);
}

Mesh::~Mesh()
{
	glDeleteBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
	glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void Mesh::InitMesh(Model& model)
{
	glGenVertexArrays(1, &m_vertexArrayObject);
	
	glBindVertexArray(m_vertexArrayObject);

	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);

	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size()*sizeof(glm::vec3), &model.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// UV
	if(hasUV)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[UV]);
		glBufferData(GL_ARRAY_BUFFER, model.uvs.size()*sizeof(glm::vec2), &model.uvs[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	//NORMAL
	if (hasNorm)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL]);
		glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(glm::vec3), &model.normals[0], GL_STATIC_DRAW);
    	glEnableVertexAttribArray(2);
    	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	//INDEX
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size()*sizeof(unsigned short), &model.indices[0] , GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::Draw()
{
	glBindVertexArray(m_vertexArrayObject);

	glEnableVertexAttribArray(0);
	
	if(hasUV)
		glEnableVertexAttribArray(1);
	
	if (hasNorm)
		glEnableVertexAttribArray(2);

	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_SHORT, (void*)0);

	glDisableVertexAttribArray(0);
	
	if(hasUV)
		glDisableVertexAttribArray(1);
	
	if (hasNorm)
		glDisableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::SendData(Model& model)
{
	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, model.vertices.size()*sizeof(glm::vec3), &model.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// UV
	if(hasUV)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[UV]);
		glBufferData(GL_ARRAY_BUFFER, model.uvs.size()*sizeof(glm::vec2), &model.uvs[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	//NORMAL
	if (hasNorm)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL]);
		glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(glm::vec3), &model.normals[0], GL_STATIC_DRAW);
    	glEnableVertexAttribArray(2);
    	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	//INDEX
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size()*sizeof(unsigned short), &model.indices[0] , GL_STATIC_DRAW);
}