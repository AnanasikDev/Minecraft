#pragma once

#include "glh.h"
#include <string>
#include <iostream>
#include <fstream>

struct Program;

struct Shader
{
	Program* m_program = nullptr;
	std::string m_source = "";
	const char* m_compiledSource = nullptr;
	GLuint id = 0;

	enum class Type
	{
		FVertex,
		Fragment
	} m_type;

	std::string GetName() const;

	static int TypeToGLType(Type type);

	Shader(Program* const m_program, Type type);

	bool LoadFromFile(const std::string& path);

	bool LoadFromString(const std::string& code);

	bool Compile();
};