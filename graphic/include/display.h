#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class Display
{
public:
	Display();

	Display( const std::string& title, int width, int height );

	void Clear(float r, float b, float g, float a);

	void Update();

	bool isClosed();

	virtual ~Display();

protected:

private:
	GLFWwindow* m_window;

	bool m_isClosed;

};

#endif // DISPLAY_H