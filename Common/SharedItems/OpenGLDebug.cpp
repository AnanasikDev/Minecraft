#include "OpenGLDebug.h"
#include "glh.h"
#include <iostream>

bool OpenGLDebug::StartErrorCapture()
{
	if (!enabled) return true;
	return ClearErrors();
}

bool OpenGLDebug::EndErrorCapture()
{
	if (!enabled) return true;
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
	if (!enabled) return true;
	GLenum error = glGetError();
	bool wasClear = error == GL_NO_ERROR;
	while (error)
	{
		error = glGetError();
	}
	return wasClear;
}