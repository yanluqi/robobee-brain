#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#include <cassert>
#include <iostream>

#include <GL/glew.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

class Texture
{
public:
	Texture();

	Texture(const std::string& fileName);
	
	~Texture();

	void SetTex(const std::string& fileName);

	void Bind(unsigned int unit);

	inline GLuint GetTexture() { return m_texture; }

	// Custom loaders (opengl-tutorial)
	GLuint loadBMP_custom(const char * imagepath);

	GLuint loadDDS(const char * imagepath);

protected:

private:
	GLuint m_texture;
	
};

#endif // TEXTURE_H