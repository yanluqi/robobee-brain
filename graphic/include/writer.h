#ifndef WRITER_H
#define WRITER_H

// Std. Includes
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cstring>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// GL includes
#include "shader.h"
#include "texture.h"


struct Character {
    GLuint     TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;       // Size of glyph
    glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
    GLuint     Advance;    // Offset to advance to next glyph
};

class Writer
{
public:
	Writer();

	~Writer();

	void getVertices(const char * text);

	void Print(std::string text);

	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

protected:

private:

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texcoords;

	int x, y, size;

	enum attribute
	{
		TEXCOORD,

		NUM_BUFFERS
	};

	enum uniform
	{
		TEX,

		COLOR,

		PROJECTION,

		NUM_UNIFORM
	};

	GLuint 	m_vertexArrayObject,
			m_vertexArrayBuffers[NUM_BUFFERS],
			uniforms[NUM_UNIFORM],
			shader_prog,
			texture;

	FT_Library ft;
	FT_Face face;

	std::map<GLchar, Character> Characters;
};

#endif // WRITER_H