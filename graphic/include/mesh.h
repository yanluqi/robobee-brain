#ifndef MESH_H
#define MESH_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "objloader.hpp"
#include "vboindexer.hpp"
#include "model.h"

class Mesh
{
public:
	Mesh();

	Mesh(const std::string& filePath, Model& model);

	Mesh(Model& model);

	void InitMesh(Model& model);

	virtual ~Mesh();

	virtual void Draw();

	virtual void SendData(Model& model);

protected:
	enum
	{
		VERTEX,

		UV,

		NORMAL,

		INDEX,

		NUM_BUFFERS		
	};

	GLuint m_vertexArrayObject;
    GLuint m_vertexArrayBuffers[NUM_BUFFERS];

    int m_drawCount;

    bool hasUV, hasNorm;

private:

};

#endif // MESH_H