#include "Shader.h"
#include <malloc.h>

std::string Shader::GetName() const
{
	switch (m_type)
	{
	case Type::Vertex:
		return "Vertex";
	case Type::Fragment:
		return "Fragment";
	default:
		throw std::invalid_argument("Unsupported shader type\n");
	}
	return 0;
}

int Shader::TypeToGLType(Type type)
{
	switch (type)
	{
	case Type::Vertex:
		return GL_VERTEX_SHADER;
	case Type::Fragment:
		return GL_FRAGMENT_SHADER;
	default:
		throw std::invalid_argument("Unsupported shader type\n");
	}
	return 0;
}

Shader::Shader(Program* const m_program, Type type) : m_type(type)
{
#if MYGL_DEFAULT_SHADERS == 1
	switch (type)
	{
	case Type::Vertex:
		m_source = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"void main()\n"
			"{\n"
			"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";
		break;
	case Type::Fragment:
		m_source = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"uniform float iScale;\n"
			"void main()\n"
			"{\n"
			"    FragColor = vec4(iScale, 0.9f, 0.8f, 1.0f);\n"
			"}\n\0";
		break;
	default:
		throw std::invalid_argument("Unsupported shader type\n");
	}
#endif
}

bool Shader::LoadFromFile(const std::string& path)
{
	std::ifstream stream(path);
	if (stream)
	{
		stream.seekg(0, stream.end);
		const std::streampos lengthPos{ stream.tellg() };
		const size_t length{ static_cast<size_t>(lengthPos) };
		m_source.resize(length);
		stream.seekg(0);
		stream.read(m_source.data(), static_cast<std::streamsize>(length));
		printf("Successfully loaded shader at %s", path.c_str());
		std::string srcpreview{ m_source };
		if (srcpreview.size() > PREVIEW_LENGTH) srcpreview[PREVIEW_LENGTH] = '\0';
		printf(" | preview: %s\n", srcpreview.c_str());
		return LoadFromString(m_source);
	}
	printf("Failed to load shader at %s\n", path.c_str());
	return false;
}

bool Shader::LoadFromString(const std::string& code)
{
	m_source = code;
	id = glCreateShader(TypeToGLType(m_type));
	m_compiledSource = code.c_str();
	glShaderSource(id, 1, &m_compiledSource, NULL);
	return true;
}

bool Shader::Compile()
{
	glCompileShader(id);
	int success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		int length = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = nullptr;
		message = (char*)malloc(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "ERROR: " << GetName() << " SHADER COMPILATION FAILED\n" << message << std::endl;
		glDeleteShader(id);
		free(message);
		return false;
	}
	return true;
}
