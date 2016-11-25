#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Model
{
public:
	Model() {}

	Model(GLfloat *positions, int numElements)
	{
		for (int i = 0; i < numElements/3; ++i)
		 {
		 	vertices.push_back( glm::vec3(positions[3*i], positions[3*i+1], positions[3*i+2]) );
		 }

		 hasUVs = false;
		 hasNormals = false;
	}

	Model(GLfloat *positions, GLfloat *texcoords, int numElements)
	{
		for (int i = 0; i < numElements/3; ++i)
		 {
		 	vertices.push_back( glm::vec3(positions[3*i], positions[3*i+1], positions[3*i+2]) );
		 	uvs.push_back( glm::vec2(texcoords[2*i], texcoords[2*i+1]) );
		 }
		 hasUVs = true;
		 hasNormals = false; 
	}

	Model(GLfloat *positions, GLfloat *texcoords, GLfloat *norms, int numElements)
	{
		for (int i = 0; i < numElements/3; ++i)
		 {
		 	vertices.push_back( glm::vec3(positions[3*i], positions[3*i+1], positions[3*i+2]) );
		 	uvs.push_back( glm::vec2(texcoords[2*i], texcoords[2*i+1]) );
		 	normals.push_back( glm::vec3(norms[3*i], norms[3*i+1], norms[3*i+2]) );
		 }

		 hasUVs = true;
		 hasNormals = true; 
	}

	void SetVertices(std::vector<glm::vec3>& vertices) 
	{
		if (!this->vertices.empty())
			this->vertices.clear();

		this->vertices = vertices; 
	}

	void SetUVs(std::vector<glm::vec2>& uvs) 
	{
		if (!this->uvs.empty())
			this->uvs.clear();

		this->uvs = uvs;
		hasUVs = true;
	}

	void SetNormals(std::vector<glm::vec3>& normals) 
	{
		if (!this->normals.empty())
			this->normals.clear();

		this->normals = normals;
		hasNormals = true;
	}

	void SetInidices(unsigned short *index, int numIndices) 
	{
		if (!indices.empty())
			indices.clear();

		for (int i = 0; i < numIndices; ++i)
		 {
		 	indices.push_back((unsigned short)index[i]);
		 }

		 indexed = true; 
	}

	void SetInidices(std::vector<unsigned short>& indices) 
	{
		if (!this->indices.empty())
			this->indices.clear();

		this->indices = indices;
		indexed = true; 
	}

	// std::vector<glm::vec3> GetVertices() { return vertices; }
	// std::vector<glm::vec2> GetTexCoords() { return uvs; }
	// std::vector<glm::vec3> GetNormals() { return normals; }

	std::vector<unsigned short> indices;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	bool hasUVs, hasNormals, indexed; 
private:

};

#endif // MODEL_H