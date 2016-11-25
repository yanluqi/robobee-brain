#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "transform.h"
#include "camera.h"
#include "light.h"

class Shader
{
public:
	Shader();

	Shader(const std::string& vertexPath, const std::string& fragmentPath);

	virtual ~Shader();

	inline GLuint GetShader() { return m_program; }
	inline GLuint GetUniform(int num) { return m_uniforms[num]; }

	void SetShader(const std::string& vertexPath, const std::string& fragmentPath);

	void Bind();

	void Update(Transform& State, Camera* Cam, Light* Lamp);

	static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

protected:

private:
	enum
	{
		MODEL,

		VIEW,

		MVP,

		LIGHTPOS,

		GSAMPLER,

		NUM_UNIFORMS
	};

	GLuint m_program;

	GLuint m_uniforms[NUM_UNIFORMS];
};

#endif // SHADER_H