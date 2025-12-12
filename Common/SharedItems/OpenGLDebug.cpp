#include "OpenGLDebug.h"
#include "glh.h"
#include <iostream>

bool OpenGLDebug::StartErrorCapture()
{
	return ClearErrors();
}

bool OpenGLDebug::EndErrorCapture()
{
	GLenum error = glGetError();
	bool wasClear = error == GL_NO_ERROR;
	while (error)
	{
		std::cerr << "DEBUG OpenGL error: " << error << std::endl;
		error = glGetError();
	}
	return wasClear;
}

bool OpenGLDebug::ClearErrors()
{
	GLenum error = glGetError();
	bool wasClear = error == GL_NO_ERROR;
	while (error)
	{
		error = glGetError();
	}
	return wasClear;
}